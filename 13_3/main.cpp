/**
  * C. Areas
  *
  * input standard input
  * output standard output
  *
  * You're given n straight lines on 2D plane.
  * They divide the plane into parts.
  * Some of them are finite and others are infinite.
  * Find areas of all finite parts.
  *
  * Input
  * First line contains integer n — the number of lines (1 ≤ n ≤ 80).
  * Each of next n lines contain four integers x 1, y 1, x 2 and y 2 — coordinates of two distinct points of i-th line.
  * Coordinates don't exceed 100 by absolute value. All lines are pairwise distinct.
  *
  * Output
  * In the first line output k — number of finite parts.
  * In next k lines output areas of these parts in non-decreasing order.
  * The error shouldn't exceed 10^-4.
  * Ignore and don't output parts with an area less than 10^-8.
  */

#define DEBUG

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>

using value_t = long long;

static constexpr value_t MAX_COORD = 200000001;
static const double MIN_SQUARE = 1e-8;

class KahanAccumulator
{
private:
    double value_;
    double delta_;
public:
    KahanAccumulator() : value_(0), delta_(0)
    {
    }
    double getValue() const
    {
        return value_;
    }
    void add(double summand)
    {
        double a = summand - delta_;
        double b = value_ + a;
        delta_ = (b - value_) - a;
        value_ = b;
    }
};

struct Point
{
    double x_;
    double y_;

    Point()
    {
    }
    Point(double x, double y) : x_(x), y_(y)
    {
    }
};

struct HalfPlane
{
    value_t a_;
    value_t b_;
    value_t c_;
    bool border_;

    HalfPlane(value_t a, value_t b, value_t c, bool border = false) : a_(a), b_(b), c_(c), border_(border)
    {
    }

    HalfPlane(value_t x0, value_t y0, value_t x1, value_t y1, bool border = false) : a_(y0 - y1), b_(x1 - x0), c_(x0 * y1 - y0 * x1), border_(border)
    {
    }

    bool belongs(const Point& p) const
    {
        return a_ * p.x_ + b_ * p.y_ + c_ >= 0;
    }
    bool intersect(const HalfPlane& other, Point& intersect) const
    {
        if (a_ == other.a_ && b_ == other.b_)
        {
            return false;
        }
        value_t detAB = a_ * other.b_ - b_ * other.a_;
        intersect.x_ = 1.0 * (b_ * other.c_ - c_ * other.b_) / detAB;
        intersect.y_ = 1.0 * (c_ * other.a_ - a_ * other.c_) / detAB;
        return true;
    }
    HalfPlane revert() const
    {
        return HalfPlane(-a_, -b_, -c_, border_);
    }
};

bool pointIsBetween(const Point& p0, const Point& p1, Point& pBetween)
{
    return ((p0.x_ <= pBetween.x_ && pBetween.x_ < p1.x_) || (p0.x_ >= pBetween.x_ && pBetween.x_ > p1.x_))
            && ((p0.y_ <= pBetween.y_ && pBetween.y_ < p1.y_) || (p0.y_ >= pBetween.y_ && pBetween.y_ > p1.y_));
}

std::pair<std::vector<Point>, std::vector<HalfPlane>> cut(
        const std::vector<Point>& polygon,
        const std::vector<HalfPlane>& polygonLines,
        const HalfPlane& hp)
{
    std::vector<Point> newPolygon;
    std::vector<HalfPlane> newPolygonLines;
    for (std::size_t i = 0; i < polygon.size(); ++i)
    {
        bool pBelongs = hp.belongs(polygon[i]);
        bool pNextBelongs = hp.belongs(polygon[(i + 1) % polygon.size()]);
        if (pBelongs)
        {
            newPolygon.push_back(polygon[i]);
            newPolygonLines.push_back(polygonLines[i]);
        }
        if (pBelongs && !pNextBelongs)
        {
            Point pIntersect;
            hp.intersect(polygonLines[i], pIntersect);
            newPolygon.push_back(pIntersect);
            newPolygonLines.push_back(hp);
        }
        if (!pBelongs && pNextBelongs)
        {
            Point pIntersect;
            hp.intersect(polygonLines[i], pIntersect);
            newPolygon.push_back(pIntersect);
            newPolygonLines.push_back(polygonLines[i]);
        }
    }
    return std::make_pair(newPolygon, newPolygonLines);
}

// p0p1 x p0p2
double calcVectorProduct(const Point& p0, const Point& p1, const Point& p2)
{
    const double x01 = p1.x_ - p0.x_;
    const double y01 = p1.y_ - p0.y_;
    const double x02 = p2.x_ - p0.x_;
    const double y02 = p2.y_ - p0.y_;
    return x01 * y02 - y01 * x02;
}

double calcConvexHullSquare(const std::vector<Point>& hull)
{
    const Point p0 = hull[0];
    KahanAccumulator square;

    for (std::size_t i = 1; i < hull.size() - 1; ++i)
    {
        const double abcSquare = 0.5 * calcVectorProduct(p0, hull[i], hull[i + 1]);
        assert(abcSquare >= 0);
        square.add(abcSquare);
    }

    return square.getValue();
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

    std::vector<Point> polygon;
    polygon.emplace_back(-MAX_COORD, -MAX_COORD);
    polygon.emplace_back( MAX_COORD, -MAX_COORD);
    polygon.emplace_back( MAX_COORD,  MAX_COORD);
    polygon.emplace_back(-MAX_COORD,  MAX_COORD);

    std::vector<HalfPlane> polygonLines;
    polygonLines.emplace_back( 0,  1, MAX_COORD, true);
    polygonLines.emplace_back(-1,  0, MAX_COORD, true);
    polygonLines.emplace_back( 0, -1, MAX_COORD, true);
    polygonLines.emplace_back( 1,  0, MAX_COORD, true);

    std::vector<std::pair<std::vector<Point>, std::vector<HalfPlane>>> data;
    data.push_back(std::make_pair(polygon, polygonLines));

    std::size_t lineCount;
    cin >> lineCount;
    for (std::size_t i = 0; i < lineCount; ++i)
    {
        value_t x0, y0, x1, y1;
        cin >> x0 >> y0 >> x1 >> y1;
        HalfPlane hp0(x0, y0, x1, y1);
        HalfPlane hp1 = hp0.revert();

        std::vector<std::pair<std::vector<Point>, std::vector<HalfPlane>>> newData;

        for (auto& datum : data)
        {
            auto newDatum0 = cut(datum.first, datum.second, hp0);
            if (newDatum0.first.size() && calcConvexHullSquare(newDatum0.first) > MIN_SQUARE)
            {
                newData.push_back(newDatum0);
            }
            auto newDatum1 = cut(datum.first, datum.second, hp1);
            if (newDatum1.first.size() && calcConvexHullSquare(newDatum1.first) > MIN_SQUARE)
            {
                newData.push_back(newDatum1);
            }
        }
        data = newData;
    }

    std::vector<double> squares;
    for (auto& datum : data)
    {
        bool isInfinite = false;
        for (HalfPlane& hp : datum.second)
        {
            if (hp.border_)
            {
                isInfinite = true;
                break;
            }
        }
        if (!isInfinite)
        {
            double square = calcConvexHullSquare(datum.first);
            if (square > MIN_SQUARE)
            {
                squares.push_back(square);
            }
        }
    }

    std::sort(squares.begin(), squares.end());
    std::cout << squares.size() << '\n';
    for (double square : squares)
    {
        std::cout << square << '\n';
    }

    return 0;
}
