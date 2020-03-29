/**
 * E. Факторизация
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дано натуральное число.
 * Факторизуйте его, то есть представьте в виде произведения набора простых чисел.
 * Число p называется простым, если имеет ровно два различных натуральных делителя: 1 и p.
 *
 * Входные данные
 * В единственной строке записано единственное натуральное число N. 2 ≤ N ≤ 9·10^18.
 *
 * Выходные данные
 * Выведите в неубывающем порядке одно или несколько простых чисел, произведение которых равно N.
 */

#define DEBUG

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <ctime>
#include <iostream>
#ifdef DEBUG
#include <chrono>
#include <fstream>
#endif
#include <random>
#include <queue>
#include <unordered_map>
#include <vector>

using value_t = unsigned long long;

value_t gcd(value_t a, value_t b)
{
   return b ? gcd(b, a % b) : a;
}

// a*b mod modulo with regard to overflow
value_t mulMod(value_t a, value_t b, value_t modulo)
{
    value_t result = 0;
    a %= modulo;
    while (b)
    {
        if (b & 1)
        {
            result = (result + a) % modulo;
        }
        a = (2 * a) % modulo;
        b >>= 1;
    }
    return result;
}

// calculate value^exp mod modulo
value_t expMod(value_t value, value_t exp, value_t modulo)
{
    value_t res = 1;

    while (exp != 0)
    {
        if ((exp & 1) != 0)
        {
            res = mulMod(res, value, modulo);
        }

        value = mulMod(value, value, modulo);
        exp >>= 1;
    }

    return res;
}

bool MRTest(value_t a, std::mt19937& rng, unsigned int rounds = 10)
{
    // a - 1 = s * 2^e2
    unsigned int e2 = 0;
    value_t s = a - 1;
    while (s % 2 == 0)
    {
        ++e2;
        s /= 2;
    }

    std::uniform_int_distribution<value_t> dist(2, a - 2);

    for (unsigned int i = 0; i < rounds; ++i)
    {
        value_t b = dist(rng);
        assert(b >= 2 && b <= a - 2);
        while (gcd(a, b) != 1)
        {
            if (b++ == a)
            {
                continue;
            }
        }

        value_t x = expMod(b, s, a);
        if (x == 1 || x == a - 1)
        {
            continue;
        }

        bool isPrime = false;
        for (unsigned int j = 1; j < e2; ++j)
        {
            x = mulMod(x, x, a);
            if (x == 1)
            {
                return false;
            }
            if (x == a - 1)
            {
                isPrime = true;
                break;
            }
        }
        if (isPrime)
        {
            continue;
        }
        return false;
    }
    return true;
}

bool testPrimality(const value_t a, std::mt19937& rng)
{
    if (a < 2)
    {
        return false;
    }
    else if (a < 4) // (a == 2 || a == 3)
    {
        return true;
    }
    else if (a % 2 == 0)
    {
        return false;
    }
    else
    {
        return MRTest(a, rng);
    }
}

value_t sqrIncMod(value_t a, value_t modulo)
{
    value_t result = 0;
    a %= modulo;
    value_t b = a;
    while (b)
    {
        if (b & 1)
        {
            result = (result + a) % modulo;
        }
        a = (2 * a) % modulo;
        b >>= 1;
    }
    return (result + 1) % modulo;
}


bool rhoPollardRun(value_t a, value_t k, value_t& factor)
{
    value_t k1 = sqrIncMod(k, a);
    factor = gcd(k1 - k, a);
    while(factor == 1)
    {
        k = sqrIncMod(k, a);
        k1 = sqrIncMod(sqrIncMod(k1, a), a);
        factor = gcd(k1 - k, a);
    }
    return factor != a;
}

std::vector<value_t> factorize(value_t a, std::mt19937& rng)
{
    std::vector<value_t> factors;

    std::queue<value_t> queue;
    queue.push(a);
    while (!queue.empty())
    {
        a = queue.front();
        queue.pop();
        if (testPrimality(a, rng))
        {
            factors.push_back(a);
            continue;
        }
        if (a % 2 == 0)
        {
            queue.push(a / 2);
            factors.push_back(2);
            continue;
        }
        if (a % 3 == 0)
        {
            queue.push(a / 3);
            factors.push_back(3);
            continue;
        }
        value_t factor;
        std::uniform_int_distribution<value_t> dist(0, a - 3);
        while (!rhoPollardRun(a, dist(rng), factor))
            ;
        assert(a % factor == 0);
        queue.push(a / factor);
        queue.push(factor);
    }

    std::sort(factors.begin(), factors.end());

    return factors;

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

    std::random_device dev;
    std::mt19937 rng(dev());


    value_t a;
    cin >> a;

#ifdef DEBUG
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int  i = 0; i < 200; ++i)
    {
#endif
        auto factors = factorize(a, rng);

        for (value_t factor : factors)
        {
            std::cout << factor << ' ';
        }
#ifdef DEBUG
        std::cout << "\n";
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    auto d = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    std::cout << "time per run " << (d / 100) << '\n';
#endif

    return 0;
}
