/**
  * A. Two Circles
  *
  * input standard input
  * output standard output
  *
  * You are given two circles.
  * Intersect given circles and find the points of their intersection.
  *
  * Input
  * First line of input contains one positive integer up to 10 000 — the number of testcases.
  * Then follow the testcases.
  * Every testcase consists of two lines.
  * Each of these lines describe one of two circles containing three integers: coordinates of center and radius of the circle.
  * All numbers are integers not exceeding 10 000 by their absolute value.
  * Radius is positive.
  *
  * Output
  * For each testcase output several lines.
  * The first line should contain the number of points of intersection: 0, 1 or 2, if it's finite, otherwise output 3.
  * If there is only one point of intersection, output its coordinates on the second line.
  * If there are two points of intersection, output coordinates of points H on the second line,
  * third line should contain two numbers: the lengths of vectors OH and HP,
  * and the next two lines — the coordinates of points of intersection.
  * You can output these two points in arbitrary order.
  *
  * Note
  * The output is not ambiguous.
  * Coordinates and lengths should be output with no less that six correct digits after the decimal point.
  * Note, that the input consists of integers, so the value "number of points of intersection" can be determined absolutely precisely.
  */

#define DEBUG

#include <cmath>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif

using value_t = long long;

class Circle
{
private:
    const value_t x_;
    const value_t y_;
    const value_t radius_;
public:
    Circle(value_t x, value_t y, value_t radius) : x_(x), y_(y), radius_(radius)
    {
    }
    void printOutIntersection(const Circle& other)
    {
        if (x_ == other.x_ && y_ == other.y_ && radius_ == other.radius_)
        {
            std::cout << "3\n";
            return;
        }
        const value_t x01 = other.x_ - x_;
        const value_t y01 = other.y_ - y_;
        const value_t length01Sq = x01 * x01 + y01 * y01;
        const value_t radiiSumSq = (radius_ + other.radius_) * (radius_ + other.radius_);
        const value_t radiiDeltaSq = (radius_ - other.radius_) * (radius_ - other.radius_);
        if (radiiSumSq < length01Sq || radiiDeltaSq > length01Sq)
        {
            std::cout << "0\n";
        }
        else if (radiiSumSq == length01Sq)
        {
            std::cout << "1\n";
            const double coeff = 1.0 * radius_ / (radius_ + other.radius_);
            const double xIs = x_ + coeff * x01;
            const double yIs = y_ + coeff * y01;
            std::cout << xIs << ' ' << yIs << '\n';
        }
        else if (radiiDeltaSq == length01Sq)
        {
            std::cout << "1\n";
            const double coeff = 1.0 * radius_ / (radius_ - other.radius_);
            const double xIs = x_ + coeff * x01;
            const double yIs = y_ + coeff * y01;
            std::cout << xIs << ' ' << yIs << '\n';
        }
        else
        {
            std::cout << "2\n";
            // P0P1 equation ax + by + c = 0
            const value_t a = 2 * x01;
            const value_t b = 2 * y01;
            const value_t c = -(radius_ * radius_ - other.radius_ * other.radius_ + other.x_ * other.x_ - x_ * x_ + other.y_ * other.y_ - y_ * y_);

            const double O0HSq = 1.0 * (a * x_ + b * y_ + c) * (a * x_ + b * y_ + c) / (a * a + b * b);
            const double HPSq = radius_ * radius_ - O0HSq;

            const double xH = (1.0 * b * (b * x_ - a * y_) - a * c) / (a * a + b * b);
            const double yH = (1.0 * a * (-b * x_ + a * y_) - b * c) / (a * a + b * b);

            std::cout << xH << ' ' << yH << '\n';
            std::cout << std::sqrt(O0HSq) << ' ' << std::sqrt(HPSq) << '\n';

            const double normCoeffHP = std::sqrt(HPSq / (a * a + b * b));
            const double xP0 = xH + normCoeffHP * b;
            const double yP0 = yH - normCoeffHP * a;
            const double xP1 = xH - normCoeffHP * b;
            const double yP1 = yH + normCoeffHP * a;

            std::cout << xP0 << ' ' << yP0 << '\n';
            std::cout << xP1 << ' ' << yP1 << '\n';
        }
    }
};

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
    std::size_t caseCount;
    cin >> caseCount;

    for (std::size_t i = 0; i < caseCount; ++i)
    {
        value_t x0, y0, r0, x1, y1, r1;
        cin >> x0 >> y0 >> r0 >> x1 >> y1 >> r1;
        Circle c0(x0, y0, r0);
        Circle c1(x1, y1, r1);
        c0.printOutIntersection(c1);
    }

    return 0;
}
