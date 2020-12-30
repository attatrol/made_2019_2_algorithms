/**
  * B. Пересечение полуплоскостей
  *
  * ввод стандартный ввод
  * вывод стандартный вывод
  *
  * Эта задача не имеет условия.
  *
  * Входные данные
  * Первая строка входного файла содержит единственное число n — число полуплоскостей, площадь пересечения которых вам требуется посчитать (3 ≤ n ≤ 2000).
  * Следующие n строк содержат по три целых числа a i, b i, c i, по модулю не превосходящих 10000, задающие полуплоскость ((x, y) принадлежит i-й полуплоскости, если a·x + b·y + c ≥ 0).
  * Гарантируется, что площадь пересечения конечна.
  *
  * Выходные данные
  * Одно вещественное число — площадь пересечения полуплоскостей с точностью до 10^-6.
  */

#define DEBUG

#include <assert.h>
#include <cmath>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>

using value_t = long long;

static const value_t MAX_COORD = 200000001;

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

    HalfPlane(value_t a, value_t b, value_t c, bool border = false) : a_(a), b_(b), c_(c)
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
        std::size_t iNext = (i + 1) % polygon.size();
        bool pBelongs = hp.belongs(polygon[i]);
        bool pNextBelongs = hp.belongs(polygon[iNext]);
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
        assert(abcSquare > 0);
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
    polygonLines.emplace_back( 0,  1, MAX_COORD);
    polygonLines.emplace_back(-1,  0, MAX_COORD);
    polygonLines.emplace_back( 0, -1, MAX_COORD);
    polygonLines.emplace_back( 1,  0, MAX_COORD);

    std::size_t halfPlaneCount;
    cin >> halfPlaneCount;
    for (std::size_t i = 0; i < halfPlaneCount; ++i)
    {
        value_t a, b, c;
        cin >> a >> b >> c;
        HalfPlane hp(a, b, c);
        auto [newPolygon, newPolygonLines] = cut(polygon, polygonLines, hp);
        polygon = newPolygon;
        polygonLines = newPolygonLines;
    }

    std::cout << calcConvexHullSquare(polygon) << '\n';
    return 0;
}
