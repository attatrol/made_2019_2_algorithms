/**
 * A. Первообразный корень по простому модулю
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дано простое нечётное число p
 * Требуется найти минимальный первообразный корень по модулю p
 *
 * Входные данные
 * Первая строка входного файла содержит простое число p
 * (3≤p≤109).
 *
 * Выходные данные
 * Выведите ответ на поставленную задачу.
 */

#define DEBUG

#include <assert.h>
#include <cmath>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>

using ull_t = unsigned long long;

// calculate value^exp mod modulo
ull_t expMod(ull_t value, ull_t exp, ull_t modulo)
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

const std::vector<ull_t> getPrimeFactors(ull_t value)
{
    assert(value > 1);
    std::vector<ull_t> factors;
    for(ull_t i = 2, sqrtValue = std::ceil(std::sqrt(value)); i <= sqrtValue || value != 1; ++i)
    {
        if (value % i == 0)
        {
            factors.push_back(i);
            while (value % i == 0)
            {
                value /= i;
            }
        }
    }
    if (value > 1)
    {
        factors.push_back(value);
    }
    return factors;
}

ull_t getPrimitiveRootOfPrime(const ull_t value) {
    const ull_t totient = value - 1;
    const std::vector<ull_t> primeFactors = getPrimeFactors(totient);

    for (ull_t i = 2; i <= value; ++i)
    {
        for (ull_t factor : primeFactors)
        {
            if (expMod(i, totient / factor, value) == 1)
            {
                goto outer_cycle_end;
            }
        }
        return i;
        outer_cycle_end:;
    }
    return 1; // suppress compiler warning
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

    ull_t value;
    cin >> value;
    assert(value > 2);
    std::cout << getPrimitiveRootOfPrime(value) << '\n';

    return 0;
}
