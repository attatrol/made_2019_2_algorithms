/**
 * B. Просеивай!
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Для положительного целого n определим функции:
 *
 * d(n)  — минимальный делитель n, больший 1, по определению положим d(1) = 0.
 * s0(n) — количество различных делителей n.
 * s1(n) — сумма всех делителей n.
 * φ(n)  — функция Эйлера, количество целых чисел k, таких что 1 ≤ k ≤ n и GCD(n, k) = 1.
 *
 * По данному числу n вычислите сумму по k от 1 до n  d(k), s0(k), s1(k) и φ(k).
 *
 * Входные данные
 * В единственной строке записано число n (1 ≤ n ≤ 107).
 *
 * Выходные данные
 * Выведите четыре числа: d(k), s0(k), s1(k) и φ(k).
 */

#define DEBUG

#include <assert.h>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <tuple>
#include <vector>

using ull_t = unsigned long long;

static const int MAX_VALUE = 10000000;

inline std::tuple<ull_t, ull_t, ull_t, ull_t> generateTuple(int* addrCache, int* factorCache, const int size)
{
    int* totient = new int[size + 1];
    ull_t* divisorSums = new ull_t[size + 1];

    ull_t totientSum = totient[1] = 1; // totient[0] not defined
    ull_t divisorCountSum = 1;
    ull_t minDivisorSum = 0;
    ull_t divisorSum = 1;

    for (int n = 2; n <= size; ++n)
    {
        if (addrCache[n] == -1)
        {
            totientSum += totient[n] = n - 1;
        }
        else
        {
            int n1 = addrCache[n];
            int p = factorCache[n];
            assert(totient[factorCache[n]] == factorCache[n] - 1);
            int t = p - 1;
            while (n1 != -1 && factorCache[n1] == p)
            {
                n1 = addrCache[n1];
                assert(n1 != 0 && n1 != 1);
                t *= p;
            }
            if (n1 == -1)
            {
                totientSum += totient[n] = t;
            }
            else
            {
                totientSum += totient[n] = t * totient[n1];
            }
        }

        ull_t divisorCount = 1;
        int currentFactorCount = 1;
        int currentFactor = factorCache[n];
        minDivisorSum += currentFactor;
        int n1 = addrCache[n];
        while (n1 != -1)
        {
            if (factorCache[n1] != currentFactor)
            {
                currentFactor = factorCache[n1];
                divisorCount *= currentFactorCount + 1;
                currentFactorCount = 1;
            }
            else
            {
                ++currentFactorCount;
            }
            n1 = addrCache[n1];
        }
        divisorCount *= currentFactorCount + 1;
        divisorCountSum += divisorCount;

        if (addrCache[n] == -1)
        {
            divisorSum += divisorSums[n] = n + 1;
        }
        else
        {
            int n1 = addrCache[n];
            ull_t currentDivisorSum = 1;
            int p0 = factorCache[n];
            int p;
            int fp = p = p0;
            while (n1 != -1)
            {
                if (factorCache[n1] == p)
                {
                    fp *= p;
                }
                else
                {
                    currentDivisorSum *= divisorSums[fp];
                    fp = p = factorCache[n1];
                }
                n1 = addrCache[n1];
            }
            if (p0 == p)
            {
                divisorSum += divisorSums[n] = ((ull_t)fp * p - 1) / (p - 1);
            }
            else
            {
                currentDivisorSum *= divisorSums[fp];
                divisorSum += currentDivisorSum;
            }
        }

    }

    delete[] totient;

    // totient:             1, 1, 2, 2, 4, 2, 6, 4, 6, 4
    // divisor counts:      1, 2, 2, 3, 2, 4, 2, 4, 3, 4
    // divisor counts sums: 1, 3, 5, 8,10,14,16,20,23,27

    return std::make_tuple(minDivisorSum, divisorCountSum, divisorSum, totientSum);
}

inline std::tuple<ull_t, ull_t, ull_t, ull_t> factorize(const int size)
{
    assert(size >= 1 && size <= MAX_VALUE);

    int* addrCache   = new int[size + 1];
    int* factorCache = new int[size + 1];
    std::fill(factorCache, factorCache + size + 1, 0);
    std::vector<int> primes;

    addrCache[0] = addrCache[1] = -1;
    factorCache[1] = 0; // by definition from the task

    for (int n = 2; n <= size; ++n)
    {
        if (factorCache[n] == 0)
        {
            factorCache[n] = n;
            primes.push_back(n);
        }
        for (int p : primes)
        {
            if (p > factorCache[n] || n * p > size)
            {
                break;
            }
            factorCache[n * p] = p;
        }
    }

    for (int n = 2; n <= size; ++n)
    {
        addrCache[n] = factorCache[n] == n ? -1 : n / factorCache[n];
    }

    auto result = generateTuple(addrCache, factorCache, size);

    delete[] addrCache;
    delete[] factorCache;

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

    int value;
    cin >> value;
    auto result = factorize(value);
    std::cout << std::get<0>(result) << ' ' << std::get<1>(result) << ' ' << std::get<2>(result) << ' ' << std::get<3>(result) << std::endl;

    return 0;
}

