/**
 * C. Корень по модулю
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Даны числа a, b и m, (1≤a,b<m≤10^9), m — простое.
 * Требуется найти корень степени b из a по модулю m.
 *
 * Входные данные
 * Входной файл состоит из одного или нескольких тестов. Количество тестов T≤300 задано в первой строке.
 * В T последующих строках заданы по три числа в каждой — a, b и m
 *
 * Выходные данные
 * Для каждого из тестов выведите какой-либо из корней степени b из a по модулю m, либо -1, если искомого корня не существует.
 * Выводимое число должно быть в пределах от -1 до m−1.
 */

//#define DEBUG

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <unordered_map>
#include <vector>

using value_t = unsigned int;
using ull_t = unsigned long long;

static const value_t MAX_VALUE = 1000000000;

// calculate value^exp mod modulo
value_t expMod(ull_t value, value_t exp, value_t modulo)
{
    ull_t res = 1;

    while (exp != 0)
    {
        if ((exp & 1) != 0)
        {
            res = (res * value) % modulo;
        }

        value = (value * value) % modulo;
        exp >>= 1;
    }

    return res;
}

ull_t expModUll(ull_t value, ull_t exp, ull_t modulo)
{
    ull_t res = 1;

    while (exp != 0)
    {
        if ((exp & 1) != 0)
        {
            res = (res * value) % modulo;
        }

        value = (value * value) % modulo;
        exp >>= 1;
    }

    return res;
}

const std::vector<value_t> getPrimeFactors(value_t value)
{
    assert(value > 1);
    std::vector<value_t> result;
    for(value_t i = 2, sqrtValue = std::ceil(std::sqrt(value)); i <= sqrtValue && value != 1; ++i)
    {
        if (value % i == 0)
        {
            result.push_back(i);
            while (value % i == 0)
            {
                value /= i;
            }
        }
    }
    if (value > 1)
    {
        result.push_back(value);
    }
    return result;
}

value_t getPrimitiveRootOfPrime(const value_t value) {
    const value_t totient = value - 1;
    const std::vector<value_t> primeFactors = getPrimeFactors(totient);

    for (value_t i = 2; i <= value; ++i)
    {
        for (value_t factor : primeFactors)
        {
            if (expMod(i, totient / factor, value) == 1)
            {
                goto outer_cycle_end;
            }
        }
        return i;
        outer_cycle_end:;
    }
    assert(false);
    return 1; // suppress compiler warning
}

// a^x≡b(mod n) -> x
bool calcDiscreteLogarithm(value_t a, value_t b, value_t modulo, ull_t& x)
{
    value_t n = std::ceil(std::sqrt(modulo));
    value_t base = expMod(a, n, modulo);

    std::unordered_map<value_t, value_t> vals;
    ull_t e = base;
    // giant steps
    for (value_t i = 1; i <= n; ++i)
    {
        auto found = vals.find(e);
        if (found == vals.end())
        {
            vals[e] = i;
        }
        e = e * base % modulo;
    }

    e = b;
    // baby steps
    for (value_t i = 0; i <= n; ++i)
    {
        auto found = vals.find(e);
        if (found != vals.end())
        {
            x = found->second * n - i;
            return true;
        }
        e = e * a % modulo;
    }
    return false;
}

// x^b≡a(mod modulo) -> x
bool calcModRoot(value_t a, value_t b, value_t m, ull_t& x)
{
    value_t g = m == 2 ? 1 : getPrimitiveRootOfPrime(m);
    ull_t y;
    bool result = calcDiscreteLogarithm(expMod(g, b, m), a, m, y);
    if (result)
    {
        x = expModUll(g, y, m);
    }
    return result;
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

    std::size_t T;
    cin >> T;
    std::unordered_map<value_t, value_t> vals;
    std::vector<value_t> factors;

    for (std::size_t i = 0; i < T; ++i)
    {
        value_t a, b, m;
        cin >> a >> b >> m;
        ull_t x;
        if (calcModRoot(a, b, m, x))
        {
#ifdef DEBUG
            std::cout << expModUll(x, b, m) << " " << a << std::endl;
#endif DEBUG
            std::cout << x;
        }
        else
        {
            std::cout << "-1";
        }
        std::cout << "\n";
    }

    return 0;
}

