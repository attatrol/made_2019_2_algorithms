/**
 * D. Проверка на простоту
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дано n натуральных чисел ai.
 * Определите для каждого числа, является ли оно простым.
 *
 * Входные данные
 * Программа получает на вход число n, 1 ≤ n ≤ 1000 и далее n чисел ai, 1 ≤ ai ≤ 10^18.
 *
 * Выходные данные
 * Если число ai простое, программа должна вывести YES, для составного числа программа должна вывести NO.
 */

//#define DEBUG

#include <assert.h>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <unordered_map>
#include <vector>

using value_t = unsigned long long;
using ull_t = unsigned long long;

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
value_t expMod(ull_t value, value_t exp, value_t modulo)
{
    ull_t res = 1;

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


ull_t getRandomBetween(value_t min, value_t max)
{
    ull_t rn = (ull_t)std::rand() | ((ull_t)std::rand() << 32);
    return min + rn % (max - min + 1);
}

bool MRTest(value_t a, unsigned int rounds = 10)
{
    // a - 1 = s * 2^e2
    unsigned int e2 = 0;
    value_t s = a - 1;
    while (s % 2 == 0)
    {
        ++e2;
        s /= 2;
    }

    for (unsigned int i = 0; i < rounds; ++i)
    {
        value_t b = getRandomBetween(2, a - 2);
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

bool testPrimality(const value_t a)
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
        return MRTest(a);
    }
}

std::vector<bool> buildSieve(value_t max)
{
    std::vector<bool> sieve(max + 1);
    for (value_t i = 2; i < max + 1; ++i)
    {
        if (!sieve[i])
        {
            for (value_t j = 2 * i; j < max + 1; j += i)
            {
                sieve[j] = true;
            }
        }
    }
    return sieve;
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

    std::srand(std::time(0));

    std::size_t n;
    cin >> n;

    for (std::size_t i = 0; i < n; ++i)
    {
        value_t a;
        cin >> a;
        std::cout << (testPrimality(a) ? "YES\n" : "NO\n");
    }

    return 0;
}



