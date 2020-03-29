/**
 * D. Прямая
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Своим уравнением Ax + By + C = 0 задана прямая на плоскости.
 * Требуется найти любую принадлежащую этой прямой точку, координаты которой — целые числа от  - 5·10^18 до 5·10^18 включительно, или выяснить что таких точек нет.
 *
 * Входные данные
 * В первой строке содержатся три целых числа A, B и C ( - 2·10^9 ≤ A, B, C ≤ 2·10^9) — соответствующие коэффициенты уравнения прямой.
 * Гарантируется, что A^2 + B^2 > 0.
 *
 * Выходные данные
 * Если искомая точка существует, выведите ее координаты, иначе выведите единственное число -1.
 */

#define DEBUG

#include <assert.h>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif

using value_t = long long;

static const value_t MIN_LIMIT = -2000000000;
static const value_t MAX_LIMIT =  2000000000;

// ax + by == gcd(a, b)
value_t gcd(value_t a, value_t b, value_t& x, value_t& y) {
    if (a == 0) {
        x = 0; y = 1;
        return b;
    }
    value_t x1, y1;
    value_t d = gcd(b%a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
    return d;
}

bool findIntegerPoint(value_t A, value_t B, value_t C, value_t& pointX, value_t& pointY)
{
    C = -C; // Ax + By + C = 0 -> Ax + By = C

    if (C == 0)
    {
        pointX = B;
        pointY = -A;
        return true;
    }
    if (C < 0)
    {
        C = -C;
        A = -A;
        B = -B;
    }

    value_t x, y;
    value_t g = gcd(A, B, x, y);
    assert(g >= MIN_LIMIT && g <= MAX_LIMIT);
    assert(x >= MIN_LIMIT && x <= MAX_LIMIT);
    assert(y >= MIN_LIMIT && y <= MAX_LIMIT);
    if (C % g != 0)
    {
        return false;
    }
    else
    {
        value_t coeff = C / g;
        assert(coeff >= MIN_LIMIT && coeff <= MAX_LIMIT);
        pointX = x * coeff;
        pointY = y * coeff;
        return true;
    }
}

int main()
{
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
    std::ios::sync_with_stdio(false);
    cin.tie(0);
    std::cout.tie(0);
#endif

    value_t A, B, C;
    cin >> A >> B >> C;

    value_t pointX, pointY;

    if (findIntegerPoint(A, B, C, pointX, pointY))
    {
        std::cout << pointX << ' ' << pointY << '\n';
    }
    else
    {
        std::cout <<  "-1\n";
    }


    return 0;
}
