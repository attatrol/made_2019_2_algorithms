/**
  * D. База в джунглях
  */

//#define DEBUG

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <random>
#include <vector>

using value_t = long long;
using fvalue_t = long double;

struct Point
{
    fvalue_t x_;
    fvalue_t y_;

    bool operator==(const Point& other)
    {
        return x_ == other.x_ && y_ == other.y_;
    }
};

struct HalfPlane
{
    value_t a_;
    value_t b_;
    value_t c_;
};

bool belongsToHalfPlane(const Point& p, const HalfPlane& hp)
{
    return hp.a_ * p.x_ + hp.b_ * p.y_ + hp.c_ >= 0;
}

bool belongsToHalfPlaneStrict(const Point& p, const HalfPlane& hp)
{
    return hp.a_ * p.x_ + hp.b_ * p.y_ + hp.c_ > 0;
}

bool calcIntersection(const HalfPlane& hp0, const HalfPlane& hp1, Point& intersect)
{
    if (hp0.a_ == hp1.a_ && hp0.b_ == hp1.b_)
    {
        return false;
    }
    fvalue_t detAB = static_cast<fvalue_t>(1.0) / (hp0.a_ * hp1.b_ - hp0.b_ * hp1.a_);
    intersect.x_ = (hp0.b_ * hp1.c_ - hp0.c_ * hp1.b_) * detAB;
    intersect.y_ = (hp0.c_ * hp1.a_ - hp0.a_ * hp1.c_) * detAB;
    return true;
}

bool pointIsBetween(Point& p0, Point& p1, Point& pBetween)
{
    return ((p0.x_ <= pBetween.x_ && pBetween.x_ <= p1.x_) || (p0.x_ >= pBetween.x_ && pBetween.x_ >= p1.x_))
            && ((p0.y_ <= pBetween.y_ && pBetween.y_ <= p1.y_) || (p0.y_ >= pBetween.y_ && pBetween.y_ >= p1.y_));
}

HalfPlane getCanonicalLineEquation(const value_t x0, const value_t y0, const value_t x1, const value_t y1)
{
    HalfPlane result;
    result.a_ = y0 - y1;
    result.b_ = x1 - x0;
    result.c_ = x0 * y1 - y0 * x1;
    return result;
}

Point findMinYPoint(const std::vector<Point>& polygon)
{
    fvalue_t yMin = polygon[0].y_;
    std::size_t result = 0;
    for(std::size_t i = 1; i < polygon.size(); ++i)
    {
        if (yMin > polygon[i].y_)
        {
            yMin = polygon[i].y_;
            result = i;
        }
    }

    return polygon[result];
}

fvalue_t calcCosine(const Point& a, const Point& b, const Point& c)
{
    const fvalue_t abx = a.x_ - b.x_;
    const fvalue_t aby = a.y_ - b.y_;
    const fvalue_t cbx = c.x_ - b.x_;
    const fvalue_t cby = c.y_ - b.y_;
    const fvalue_t dotProduct = abx * cbx + aby * cby;
    const fvalue_t result = dotProduct / (std::sqrt(abx * abx + aby * aby) * std::sqrt(cbx * cbx + cby * cby));
    assert(std::fabs(result) <= 1.0);
    return result;
}

bool firstAngleIsSmaller(fvalue_t c0, fvalue_t c1)
{
    if (c0 == c1)
    {
        return true;
    }
    if (c0 >= 0 && c1 >= 0)
    {
        return c0 > c1;
    }
    if (c0 >= 0 && c1 < 0)
    {
        return true;
    }
    if (c0 < 0 && c1 >= 0)
    {
        return false;
    }
    return c0 < c1;
}

fvalue_t distance(const Point& p0, const Point& p1)
{
    const fvalue_t dx = p0.x_ - p1.x_;
    const fvalue_t dy = p0.y_ - p1.y_;
    return std::sqrt(dx * dx + dy * dy);
}

static std::random_device RD;
static std::default_random_engine RAND(242432432/*RD()*/);
const static fvalue_t EPS = 1e-9;

bool existsContinuousCut(std::vector<Point> polygon, std::size_t continuousCutSize)
{
    const std::size_t size = polygon.size();
    std::vector<std::pair<HalfPlane, std::size_t>> cutLines(size);
    for (std::size_t i = 0; i < size; ++i)
    {
        std::size_t j = i + continuousCutSize + 1;
        if (j > size - 1)
        {
            j -= size;
        }
        cutLines[i] = std::make_pair(getCanonicalLineEquation(polygon[i].x_, polygon[i].y_, polygon[j].x_, polygon[j].y_), i);
    }
    std::shuffle(cutLines.begin(), cutLines.end(), RAND);
#ifdef DEBUG
    for (std::size_t i = 0; i < size; ++i)
    {
        const HalfPlane& hp = cutLines[i].first;
        std::cout << i << ": " << hp.a_ << "x + " << hp.b_ << "y + " << hp.c_ << " >= 0\n";
    }
#endif
    Point minYPoint = findMinYPoint(polygon);

    for (std::size_t i = 0; i < size; ++i)
    {
        const HalfPlane& cutLine = cutLines[i].first;
        if (!belongsToHalfPlaneStrict(minYPoint, cutLine))
        {
            const std::size_t cutLineIndex = cutLines[i].second;
            std::size_t j = cutLineIndex + continuousCutSize + 1;
            if (j > size - 1)
            {
                j -= size;
            }
            Point lowBound = polygon[cutLineIndex];
            Point highBound = polygon[j];
            fvalue_t lowBoundCos = -1.0;
            fvalue_t highBoundCos = 1.0;
            if (lowBound.y_ > highBound.y_)
            {
                std::swap(lowBound, highBound);
            }
            Point axisHighBound = highBound;
            axisHighBound.x_ += highBound.x_ - lowBound.x_;
            axisHighBound.y_ += highBound.y_ - lowBound.y_;
            for (std::size_t k = 0; k < i; ++k)
            {
                const HalfPlane& olderCutLine = cutLines[k].first;
                Point intersect;
                if (calcIntersection(cutLine, olderCutLine, intersect) && pointIsBetween(lowBound, highBound, intersect))
                {
                    Point a;
                    a.y_ = intersect.y_ + 100;
                    a.x_ = olderCutLine.a_ ? -(olderCutLine.b_ * a.y_ + olderCutLine.c_) / olderCutLine.a_ : intersect.x_;
                    fvalue_t cosine = calcCosine(a, intersect, axisHighBound);
                    if (!belongsToHalfPlane(axisHighBound, olderCutLine))
                    {
                        if (highBound.y_ < intersect.y_ || (highBound == intersect/* && firstAngleIsSmaller(highBoundCos, cosine)*/))
                        {
                            continue;
                        }
                        highBound = intersect;
                        highBoundCos = cosine;
                    }
                    else
                    {
                        if (lowBound.y_ > intersect.y_  || (lowBound == intersect/* && !firstAngleIsSmaller(lowBoundCos, cosine)*/))
                        {
                            continue;
                        }
                        lowBound = intersect;
                        lowBoundCos = cosine;
                    }
                }
            }
            if (highBound.y_ < lowBound.y_)
            {
#ifdef DEBUG
                std::cout << "i = " << i << " failure " << continuousCutSize << '\n';
#endif
                return false;
            }
//            if (distance(highBound, lowBound) < EPS && firstAngleIsSmaller(lowBoundCos, highBoundCos))
//            {
//#ifdef DEBUG
//                std::cout << "i = " << i << " failure " << continuousCutSize << '\n';
//#endif
//                return false;
//            }
            minYPoint = lowBound;
#ifdef DEBUG
            std::cout << "i = " << i << "  " << continuousCutSize << " (" << minYPoint.x_ << "; " << minYPoint.y_ << ")\n";
#endif
        }
    }
#ifdef DEBUG
    std::cout << "success " << continuousCutSize << " (" << minYPoint.x_ << "; " << minYPoint.y_ << ")\n";
#endif
    return true;
}

int main()
{
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    std::cout.tie(nullptr);
#endif
    std::cout.precision(12);

    std::size_t pointCount;
    cin >> pointCount;

    if (pointCount <= 4)
    {
        std::cout << "1\n";
        return 0;
    }

    std::vector<Point> polygon(pointCount);
    for (std::size_t i = 0; i < pointCount; ++i)
    {
        cin >> polygon[pointCount - i - 1].x_ >> polygon[pointCount - i - 1].y_;
    }

    std::size_t cutSizeMin = 1;
    std::size_t cutSizeMax = polygon.size() / 2 + 1;
    while (cutSizeMax != cutSizeMin)
    {
        const std::size_t cutSize = (cutSizeMax + cutSizeMin) / 2;
        if (existsContinuousCut(polygon, cutSize))
        {
            cutSizeMin = cutSize + 1;
        }
        else
        {
            cutSizeMax = cutSize;
        }
        assert(cutSizeMin <= cutSizeMax);
    }
    std::cout << cutSizeMax << '\n';

    return 0;
}
