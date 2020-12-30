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
#include <time.h>
#include <vector>

using value_t = int64_t;
using fvalue_t = double;

static const fvalue_t EPS = 1e-8;

struct Point
{
    fvalue_t x_;
    fvalue_t y_;

    Point() : x_(0), y_(0)
    {
    }
    Point(fvalue_t x, fvalue_t y) : x_(x), y_(y)
    {
    }
    Point(Point from, Point to) : Point(to.x_ - from.x_, to.y_ - from.y_)
    {
    }
    bool operator == (const Point& p) const&
    {
        return std::abs(x_ - p.x_) < EPS && std::abs(y_ - p.y_) < EPS;
    }
    Point operator -() const
    {
        return Point(-x_, -y_);
    }
    Point operator +(const Point& rhs) const
    {
        return Point(-rhs, *this);
    }
    Point operator -(const Point& rhs) const
    {
        return (*this) + -rhs;
    }
    Point& operator *=(fvalue_t multiplier)
    {
        x_ *= multiplier;
        y_ *= multiplier;
        return *this;
    }
    Point& operator /=(fvalue_t divisor)
    {
        x_ /= divisor;
        y_ /= divisor;
        return *this;
    }
    Point operator /(fvalue_t divisor) const
    {
        return Point(*this) /= divisor;
    }
    fvalue_t dotProduct(const Point& other) const
    {
        return x_ * other.y_ - y_ * other.x_;
    }
    fvalue_t scalarProduct(const Point& other) const
    {
        return x_ * other.x_ + y_ * other.y_;
    }
    fvalue_t lengthSquared() const
    {
        return x_ * x_ + y_ * y_;
    }
    fvalue_t cos(const Point& other) const
    {
        return scalarProduct(other) / sqrt(static_cast<fvalue_t>(lengthSquared() * other.lengthSquared()));
    }
};

template <typename T>
struct Line
{
    T a_;
    T b_;
    T c_;

    Line(T a, T b, T c) : a_(a), b_(b), c_(c)
    {
    }
    Line() : Line(0, 0, 0)
    {
    }
    Line(Point a, Point b) : Line(a.y_ - b.y_, b.x_ - a.x_, a.dotProduct(b))
    {
    }
    bool operator ==(const Line& other) const
    {
        if (a_ != 0)
        {
            return (abs(1.0 * other.b_ * a_ / other.a_ - b_) < EPS) && (abs(1.0 * other.c_ * a_ / other.a_ - c_) < EPS);
        }
        else if (b_ != 0)
        {
            return (abs(1.0 * other.a_ * b_ / other.b_ - a_) < EPS) && (abs(1.0 * other.c_ * b_ / other.b_ - c_) < EPS);
        }
        else if (c_ != 0)
        {
            return (abs(1.0 * other.a_ * c_ / other.c_ - a_) < EPS) && (abs(1.0 * other.b_ * c_ / other.c_ - b_) < EPS);
        }
        return false;
    }
    Line operator -() const
    {
        return { -a_, -b_, -c_ };
    }
    bool isCollinear(const Line& other) const
    {
        if (a_ != 0)
        {
            return (abs(1.0 * other.b_ * a_ / other.a_ - b_) < EPS);
        }
        else if (b_ != 0)
        {
            return (abs(1.0 * other.a_ * b_ / other.b_ - a_) < EPS);
        }
        return false;
    }
    static Line getNormalBisectorisector(const Point& p0, const Point& p1)
    {
        T a = p0.x_ - p1.x_;
        T b = p0.y_ - p1.y_;
        Point M(p0 + (p1 - p0) / 2);
        T c = -a * M.x_ - b * M.y_;
        return Line(a, b, c);
    }
    std::pair<Point, fvalue_t> intersectEx(const Line& other) const
    {
        const auto& me = *this;
        fvalue_t det_ab = static_cast<fvalue_t>(me.a_ * other.b_ - me.b_ * other.a_);
        return std::make_pair(Point((me.b_ * other.c_ - me.c_ * other.b_) / det_ab, (me.c_ * other.a_ - me.a_ * other.c_) / det_ab), det_ab);
    }
    Point intersect(const Line &other) const
    {
        return intersectEx(other).first;
    }
};

struct HalfPlane : Line<value_t>
{
    using Line<value_t>::Line;
    HalfPlane(const Line<value_t>& line) : Line<value_t>(line)
    {
    }
    bool hasPoint(const Point& p) const
    {
        return this->a_ * p.x_ + this->b_ * p.y_ + this->c_ >= -EPS;
    }
};

struct Ray
{
    HalfPlane hp_;
    Point p_;
    bool sign_;
    fvalue_t cos_;

    Ray(HalfPlane _hp, Point _p, bool _sign, fvalue_t _cos) : hp_(_hp), p_(_p), sign_(_sign), cos_(_cos)
    {
    }
    Ray(HalfPlane hp0, HalfPlane ihp) : hp_(hp0)
    {
        auto intersection = hp0.intersectEx(ihp);
        p_ = intersection.first;
        sign_ = std::signbit(intersection.second);
        Point support(hp0.b_, -hp0.a_);
        cos_ = support.cos(Point(ihp.b_, -ihp.a_));
    }
    bool operator<(const Ray& rhs) const
    {
        return (compare(rhs) == -1);
    }
    bool operator<=(const Ray& rhs) const
    {
        return (compare(rhs) != 1);
    }
    bool operator==(const Ray& rhs) const
    {
        return (compare(rhs) == 0);
    }
    bool operator>(const Ray& rhs) const
    {
        return !(*this <= rhs);
    }
    bool operator>=(const Ray& rhs) const
    {
        return !(*this < rhs);
    }
    bool operator!=(const Ray& rhs) const
    {
        return !(*this == rhs);
    }
    int compare(const Ray& rhs) const
    {
        if (p_ == rhs.p_)
        {
            if (cos_ < rhs.cos_ + EPS)
            {
                return -1;
            }
            if (cos_ > rhs.cos_ - EPS)
            {
                return 1;
            }
            return 0;
        }
        Point n(hp_.a_, hp_.b_);
        Point v(p_, rhs.p_);
        return n.dotProduct(v) < 0 ? -1 : 1;
    }
};

struct Polygon
{
    std::size_t pointCount_;
    Point minYPoint_;
    std::vector<std::size_t> indexes_;
    std::vector<Point> points_;
    std::vector<std::pair<HalfPlane, std::size_t>> hps_;

    Polygon(std::size_t pointCount) : pointCount_(pointCount), points_(pointCount), indexes_(pointCount), hps_(pointCount)
    {
    }
    Point findMinYPoint()
    {
        Point minP = points_[0];
        for (std::size_t i = 1; i < points_.size(); ++i)
        {
            if (points_[i].y_ < minP.y_)
            {
                minP = points_[i];
            }
        }
        return minP;
    }
    bool findMinYIntersectPoint(std::size_t i, std::size_t step)
    {
        const auto& hp0 = hps_[i].first;
        std::size_t firstIndex = hps_[i].second;
        std::size_t secondIndex = (firstIndex + step) % pointCount_;
        Ray firstForward(hp0, points_[firstIndex], true, -1);
        Ray lastBackward(hp0, points_[secondIndex], false, 1);
        for (std::size_t j = 0; j < i; ++j) {
            const auto& hp = hps_[j].first;
            Ray b(hp0, hp);
            if (b.sign_ && b > firstForward)
            {
                firstForward = b;
            } else if (!b.sign_ && b < lastBackward)
            {
                lastBackward = b;
            }
            if (firstForward >= lastBackward)
            {
                return false;
            }
        }
        const auto p_l = firstForward.p_;
        const auto p_r = lastBackward.p_;
        minYPoint_ = p_l.y_ < p_r.y_ ? p_l : p_r;
        return true;
    }
    bool checkIntersectPointExists(std::size_t cutCount)
    {
        const std::size_t step = cutCount + 1;
        prepareHalfplanes(step);
        for (std::size_t i = 0; i < pointCount_; ++i)
        {
            if (!hps_[i].first.hasPoint(minYPoint_) && !findMinYIntersectPoint(i, step))
            {
                return false;
            }
        }
        return true;
    }
    void prepareHalfplanes(std::size_t step)
    {
        for (std::size_t i = 0; i < pointCount_; ++i)
        {
            auto idx = indexes_[i];
            hps_[idx].first = { points_[i], points_[(i + step) % pointCount_] };
            hps_[idx].second = i;
        }
    }
    void prepare()
    {
        std::reverse(points_.begin(), points_.end());
        std::iota(indexes_.begin(), indexes_.end(), 0);
        srand(time(nullptr));
        auto seed = rand();
        std::shuffle(indexes_.begin(), indexes_.end(), std::default_random_engine(seed));
        minYPoint_ = findMinYPoint();
    }
    std::size_t getHalfSize()
    {
        return (points_.size() + 1) / 2 - 1;
    }
};

int main() {
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    std::cout.tie(nullptr);
#endif
    std::size_t pointCount;
    cin >> pointCount;
    Polygon polygon(pointCount);
    for (auto& point : polygon.points_)
    {
        cin >> point.x_ >> point.y_;
    }
    polygon.prepare();

    std::size_t r = polygon.getHalfSize();
    std::size_t l = 1;
    while (r != l)
    {
        std::size_t m = l + (r - l) / 2;
        if (polygon.checkIntersectPointExists(m))
        {
            l = m + 1;
        }
        else
        {
            r = m;
        }
    }

    std::cout << r << '\n';

    return 0;
}
