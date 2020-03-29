/**
 * E. Китайская теорема
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Решите в целых числах систему уравнений
 * {x≡a(modn)
 * {x≡b(modm), где n и m взаимно просты.
 * Среди решений следует выбрать наименьшее неотрицательное число.
 *
 * Входные данные
 * Первая строка входных данных содержит число N, 1≤N≤10^4 — количество тестов, для которых нужно решить задачу.
 * Следующие N строк содержат по четыре целых числа ai, bi, ni и mi (1≤ni,mi≤10^9, 0≤ai<ni, 0≤bi<mi).
 *
 * Выходные данные
 * Для каждого из тестов выведите искомое наименьшее неотрицательное число xi
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

// borrowed from task D as is
bool findIntegerPoint(value_t A, value_t B, value_t C, value_t& pointX, value_t& pointY)
{
    C = -C; // Ax + By + C = 0 -> Ax + By = C

    if (C == 0)
    {
        pointX = B;
        pointY = -A;
        return true;
    }
    if (A == 0)
    {
        if (C % B == 0)
        {
            pointX = 0;
            pointY = C / B;
            return true;
        }
        else
        {
            return false;
        }
    }
    if (B == 0)
    {
        if (C % A == 0)
        {
            pointX = C / A;
            pointY = 0;
            return true;
        }
        else
        {
            return false;
        }
    }

    // make everything positive
    if (C < 0)
    {
        C = -C;
        A = -A;
        B = -B;
    }
    bool negativeA = A < 0;
    if (negativeA)
    {
        A = -A;
    }
    bool negativeB = B < 0;
    if (negativeB)
    {
        B = -B;
    }

    value_t x, y;
    value_t g = gcd(A, B, x, y);
    assert(g > 0 && g <= MAX_LIMIT);
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
        if (negativeA)
        {
            pointX = -pointX;
        }
        if (negativeB)
        {
            pointY = -pointY;
        }
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

    std::size_t caseCount;
    cin >> caseCount;

    for (std::size_t i = 0; i < caseCount; ++i)
    {
        value_t b0, b1, m0, m1;
        cin >> b0 >> b1 >> m0 >> m1;

        // x = y0 m0 + b0
        // x = y1 m1 + b1
        //
        // y0 m0 + b0 = y1 m1 + b1
        //
        // y0 m0 + (-m1) y1 + (b0 - b1) = 0
        //
        // just reuse code from task D:

        value_t y0, y1;
        bool hasSolution = findIntegerPoint(m0, -m1, b0 - b1, y0, y1);
        assert(hasSolution);

        value_t normalizingCoeff = y0 / m1;
        value_t normalizingSummand = normalizingCoeff * m1;
        value_t y0mod = y0 - normalizingSummand;

        value_t x = y0mod * m0 + b0;
        assert((x - b0) / m0 == y0mod); // no overflow
        value_t period = m0 * m1;
        if (x < 0)
        {
            x += period;
        }
        else if (x - period >= 0)
        {
            x -= period;
        }

        assert(x >= 0 && x <= period);
        assert(x % m0 == b0);
        assert(x % m1 == b1);

        std::cout << x << '\n';
    }

    return 0;
}
