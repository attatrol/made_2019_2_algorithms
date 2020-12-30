/**
 * E. Покрытие кругом
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Задано N точек на плоскости.
 * Найдите круг минимального радиуса, который покрывает все заданные точки.
 *
 * Входные данные
 * В первой строке входного файла содержится число N — количество точек (1 ≤ N ≤ 100 000).
 * Далее содержатся координаты точек — пара целых чисел, не превосходящих по модулю 10^6.
 *
 * Выходные данные
 * В первой строке выходных данных выведите координаты центра.
 * На следующей строке выведите одно число — радиус круга.
 * Ваш ответ будет считаться правильным, если его абсолютная или относительная погрешность не будет превышать 10^-6.
 */

#define DEBUG

#include <assert.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <random>
#include <unordered_map>
#include <vector>

using value_t = long long;

const double EPS = 1e-9;

double calcDistanceSq(double a, double b)
{
    return a * a + b * b;
}
struct Point {
    double x_;
    double y_;

    Point() : x_(0), y_(0)
    {
    }
    Point(double _x, double _y) : x_(_x), y_(_y)
    {
    }
    double dot(const Point& other) const
    {
        return x_ * other.y_ - y_ * other.x_;
    }
    double distanceSq(const Point& other)
    {
        double dX = x_ - other.x_;
        double dY = y_ - other.y_;
        return dX * dX + dY * dY;
    }
};

struct Line {
    double a_;
    double b_;
    double c_;
    Line() : Line(0, 0, 0)
    {
    }
    Line(double a, double b, double c) : a_(a), b_(b), c_(c)
    {
    }

    Line(Point a, Point b) : Line(a.y_ - b.y_, b.x_ - a.x_, a.dot(b))
    {
    }
    bool operator == (const Line& other) const&
    {
        if (a_ != 0)
        {
            return ((abs(1.0 * other.b_ * a_ / other.a_ - b_) < EPS) && (abs(1.0 * other.c_ * a_ / other.a_ - c_) < EPS));
        } else if (b_ != 0)
        {
            return ((abs(1.0 * other.a_ * b_ / other.b_ - a_) < EPS) && (abs(1.0 * other.c_ * b_ / other.b_ - c_) < EPS));
        } else if (c_ != 0)
        {
            return ((abs(1.0 * other.a_ * c_ / other.c_ - a_) < EPS) && (abs(1.0 * other.b_ * c_ / other.c_ - b_) < EPS));
        }
        return false;
    }
    static Line getNormalBisector(const Point& p0, const Point& p1)
    {
        double a = p0.x_ - p1.x_;
        double b = p0.y_ - p1.y_;
        Point M(p0.x_ + (p1.x_ - p0.x_) / 2, p0.y_ + (p1.y_ - p0.y_) / 2);
        double c = -a * M.x_ - b * M.y_;
        return Line(a, b, c);
    }
    std::pair<Point, double> intersectEx(const Line& other) const
    {
        const auto& l1 = *this;
        assert(l1.a_ != other.a_ || l1.b_ != other.b_);
        double det_ab = static_cast<double>(l1.a_ * other.b_ - l1.b_ * other.a_);
        return std::make_pair(Point((l1.b_ * other.c_ - l1.c_ * other.b_) / det_ab,
                                    (l1.c_ * other.a_ - l1.a_ * other.c_) / det_ab),
                              det_ab);
    }
    Point intersect(const Line& other) const
    {
        return intersectEx(other).first;
    }
};

struct Circle
{
    double x_;
    double y_;
    double radius_;
    double radiusSquared_;

    Circle(const Point& a, const Point& b)
    {
        Point c;
        c.x_ = (b.x_ - a.x_) / 2;
        c.y_ = (b.y_ - a.y_) / 2;
        reset(a, c);
    }
    Circle(const Point& a, const Point& b, const Point& c)
    {
        Point center(Line::getNormalBisector(a, b).intersect(Line::getNormalBisector(a, c)));
        x_ = center.x_;
        y_ = center.y_;
        radiusSquared_ = center.distanceSq(a);
        radius_ = std::sqrt(radiusSquared_);
    }
    void reset(const Point& start, const Point dst)
    {
        x_ = start.x_ + dst.x_;
        y_ = start.y_ + dst.y_;
        radiusSquared_ = dst.x_ * dst.x_ + dst.y_ * dst.y_;
        radius_ = std::sqrt(radiusSquared_);
    }
    bool includes(const Point& p) const
    {
        double deltaX = p.x_ - x_;
        double deltaY = p.y_ - y_;
        double rangeSq = deltaX * deltaX + deltaY * deltaY;
        return radiusSquared_ > rangeSq;
    }
};

Circle findMinCoverageCircleTwoPoints(std::vector<Point>& points, std::size_t q1i, std::size_t q2i)
{
    Circle result(points[q1i], points[q2i]);
    for (std::size_t i = 0; i < q1i; ++i)
    {
        if (!result.includes(points[i]))
        {
            result = Circle(points[i], points[q1i], points[q2i]);
        }
    }
    return result;
}

static std::random_device RD;
static std::default_random_engine RAND(RD());

Circle findMinCoverageCircle(std::vector<Point>& points, std::size_t qi)
{
    std::shuffle(points.begin(), points.begin() + qi, RAND);
    Circle result(points[0], points[qi]);
    for (std::size_t i = 1; i < qi; ++i)
    {
        if (!result.includes(points[i]))
        {
            result = findMinCoverageCircleTwoPoints(points, i, qi);
        }
    }
    return result;
}

Circle findMinCoverageCircle(std::vector<Point>& points)
{
    std::shuffle(points.begin(), points.end(), RAND);
    Circle result(points[0], points[1]);
    for (std::size_t i = 2; i < points.size(); ++i)
    {
        if (!result.includes(points[i]))
        {
            result = findMinCoverageCircle(points, i);
        }
    }
    return result;
}

int main() {
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
    std::vector<Point> points(pointCount);
    for (auto& point : points)
    {
        cin >> point.x_ >> point.y_;
    }

    auto result = findMinCoverageCircle(points);
    std::cout << result.x_ << " " << result.y_ << "\n" << result.radius_;

    return 0;
}
