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
using fvalue_t = double;

static const fvalue_t EPS = 1e-9;

template <typename T>
inline constexpr T sqr(T v) {
    return v * v;
}

template <typename T>
struct Point {
    Point() : x(0), y(0)
    {
    }
    Point(T _x, T _y) : x(_x), y(_y)
    {
    }
    Point(Point from, Point to) : x(to.x - from.x), y(to.y - from.y)
    {
    }
//    template <typename U>
//    operator Point<U>() const
//    {
//        return { static_cast<U>(x), static_cast<U>(y) };
//    }
    bool operator == (const Point& p) const& { return abs(x - p.x) < EPS && abs(y - p.y) < EPS; }
    Point operator -() const& { return Point(-x, -y); }
    Point operator + (const Point& rhs) const& { return Point(-rhs, *this); }
    Point operator - (const Point& rhs) const& { return (*this) + -rhs; }
    Point& operator *= (T multiplier) {
        x *= multiplier;
        y *= multiplier;
        return *this;
    }
    Point& operator /= (T divisor) { // if Point is of integral type, require chords to be divisable by divisor
        assert(!std::is_integral<T>() || ((x / divisor) * divisor == x && (y / divisor) * divisor == y));
        x /= divisor;
        y /= divisor;
        return *this;
    }
    Point operator / (T divisor) const& { return Point(*this) /= divisor; }
    friend std::istream& operator >> (std::istream& in, Point& p) { return in >> p.x >> p.y; }
    friend std::ostream& operator << (std::ostream& out, Point p) { return out << p.x << " " << p.y; }
    T dot(const Point& b) const& { return x * b.y - y * b.x; }
    T scalar(const Point& b) const& { return x * b.x + y * b.y; }
    T sqr_length() const& { return sqr(x) + sqr(y); }
    template <typename U>
    fvalue_t cos(const Point<U>& b) const& { return scalar(b) / sqrt(static_cast<fvalue_t>(sqr_length() * b.sqr_length())); }

    T x, y;
};
using IPoint = Point<value_t>;
using DPoint = Point<fvalue_t>;

template <typename T>
struct Line {
    Line(T _a, T _b, T _c) : a(_a), b(_b), c(_c) {}
    Line() : Line(0, 0, 0) {}
    Line(Point<T> a, Point<T> b) : Line(a.y - b.y, b.x - a.x, a.dot(b)) {}
    bool operator == (const Line& l2) const& {
        if (a != 0) {
            return ((abs(1.0 * l2.b * a / l2.a - b) < EPS) && (abs(1.0 * l2.c * a / l2.a - c) < EPS));
        } else if (b != 0) {
            return ((abs(1.0 * l2.a * b / l2.b - a) < EPS) && (abs(1.0 * l2.c * b / l2.b - c) < EPS));
        } else if (c != 0) {
            return ((abs(1.0 * l2.a * c / l2.c - a) < EPS) && (abs(1.0 * l2.b * c / l2.c - b) < EPS));
        }
        return false;
    }
    Line operator -() const& { return { -a, -b, -c }; }
    friend std::istream& operator >> (std::istream& in, Line& l) {
        return in >> l.a >> l.b >> l.c;
    }
    bool is_collinear(const Line& l2) const& {
        if (a != 0) {
            return (abs(1.0 * l2.b * a / l2.a - b) < EPS);
        } else if (b != 0) {
            return (abs(1.0 * l2.a * b / l2.b - a) < EPS);
        }
        return false;
    }
    static Line get_perpendicular_bisector(Point<T> A, Point<T> B) {
        T a = A.x - B.x;
        T b = A.y - B.y;
        Point M(A + (B - A) / 2);
        T c = -a * M.x - b * M.y;
        return Line(a, b, c);
    }
    std::pair<DPoint, fvalue_t> get_intersection_details(const Line& l2) const& {
        const auto& l1 = *this;
        assert(l1.a != l2.a || l1.b != l2.b);
        fvalue_t det_ab = static_cast<fvalue_t>(l1.a * l2.b - l1.b * l2.a);
        return std::make_pair(Point((l1.b * l2.c - l1.c * l2.b) / det_ab,
                                    (l1.c * l2.a - l1.a * l2.c) / det_ab),
                              det_ab);
    }
    DPoint get_intersection(const Line& l2) const& {
        return get_intersection_details(l2).first;
    }

    T a, b, c;
};
using ILine = Line<value_t>;
using DLine = Line<fvalue_t>;

template <typename T>
struct Circle {
    Circle(const Point<T>& _o, const T _r_sq) : o(_o), r_sq(_r_sq), r(sqrt(r_sq)) {}
    Circle() : Circle(Point<T>(), 0) {}
    template <typename U>
    Circle(const Point<U>& a, const Point<U>& b) : // a, b are on diameter
        o(Point<T>(a, b) / 2 + a),
        r_sq((o - a).sqr_length()),
        r(sqrt(r_sq)) {}
    template <typename U>
    Circle(const Point<U>& a, const Point<U>& b, const Point<U>& c) : // a, b, c are not collinear
        o(Line<U>::get_perpendicular_bisector(a, b).get_intersection(Line<U>::get_perpendicular_bisector(a, c))),
        r_sq((o - a).sqr_length()),
        r(sqrt(r_sq)) {}
    template <typename U>
    Circle& operator /= (U divisor) {
        o /= divisor;
        r /= divisor;
        return *this;
    }
    template <typename U>
    Circle operator / (U divisor) const& { return Circle(*this) /= divisor; }
    friend std::istream& operator >> (std::istream& in, Circle& c) { return in >> c.o >> c.r; }
    friend std::ostream& operator << (std::ostream& out, Circle c) { return out << c.o << "\n" << c.r; }
    bool includes(const IPoint& P) const
    {
        return r_sq + EPS > DPoint(o, P).sqr_length();
    }

    Point<T> o;
    T r_sq;
    T r;
};
using DCircle = Circle<fvalue_t>;

static std::random_device rd;
static std::default_random_engine random_engine(rd());

DCircle min_disc_with_two_points(std::vector<IPoint>& points, size_t q1i, size_t q2i) {
    DCircle D(points[q1i], points[q2i]);
    for (size_t i = 0; i < q1i; ++i) {
        if (!D.includes(points[i])) {
            D = DCircle(points[i], points[q1i], points[q2i]);
        }
    }
    return D;
}

DCircle min_disc_with_point(std::vector<IPoint>& points, size_t qi) {
    std::shuffle(points.begin(), points.begin() + qi, random_engine);
    DCircle D(points[0], points[qi]);
    for (size_t i = 1; i < qi; ++i) {
        if (!D.includes(points[i])) {
            D = min_disc_with_two_points(points, i, qi);
        }
    }
    return D;
}

DCircle min_disc(std::vector<IPoint>& points) {
    std::shuffle(points.begin(), points.end(), random_engine);
    DCircle D(points[0], points[1]);
    for (size_t i = 2; i < points.size(); ++i) {
        if (!D.includes(points[i])) {
            D = min_disc_with_point(points, i);
        }
    }
    return D;
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

    size_t pointCount;
    cin >> pointCount;
    std::vector<IPoint> points(pointCount);
    for (auto& point : points)
    {
        cin >> point;
        point *= 2;
    }
    std::cout << min_disc(points) / 2;

    return 0;
}
