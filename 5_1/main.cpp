/**
 * A. Сумма простая
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Вам нужно научиться отвечать на запрос «сумма чисел на отрезке».
 * Массив не меняется.
 * Запросов много.
 * Отвечать на каждый запрос следует за O(1).
 *
 * Входные данные
 * Размер массива — n и числа x, y, a0, порождающие массив a: a_i = (x * a_{i - 1} + y) mod 2^16
 * Далее следует количество запросов m и числа z, t, b0, порождающие массив b: b_i = (z * b_{i - 1} + t) mod 2^30.
 * Массив c строится следующим образом: c_i = b_i mod n.
 * Запросы: i-й из них — найти сумму на отрезке от min(c+{2i}, c_{2i + 1}) до max(c_{2i}, c_{2i + 1}) в массиве a.
 * Ограничения: 1 ≤ n ≤ 107, 0 ≤ m ≤ 107. Все числа целые от 0 до 2^16. t может быть равно  - 1.
 *
 * Выходные данные
 * Выведите сумму всех сумм.
 *
 * Пример
 *
 * Входные данные
 * 3 1 2 3
 * 3 1 -1 4
 *
 * Выходные данные
 * 23
 *
 * Примечание
 * a = {3, 5, 7},
 * b = {4, 3, 2, 1, 0, 2^30 - 1},
 * c = {1, 0, 2, 1, 0, 0},
 * запросы = {[0, 1], [1, 2], [0, 0]},
 * суммы = {8, 12, 3}.
 */

#define DEBUG

#include <assert.h>
#include <iostream>
#ifdef DEBUG
#include <chrono>
#include <fstream>
#endif
#include <vector>

using value_t = long long;

value_t* buildData(const std::size_t n, const value_t x, const value_t y, value_t a)
{
    static const value_t MASK = (1 << 16) - 1;

    value_t* result = new value_t[n];
    for (std::size_t i = 0; i < n; ++i, a = (x * a + y) & MASK)
    {
        result[i] = a;
    }
    return result;
}

std::size_t* buildRequests(const std::size_t m, const std::size_t n, const std::size_t z, const value_t t, std::size_t b)
{
    static const value_t MASK = (1 << 30) - 1;

    std::size_t* result = new std::size_t[2 * m];
    std::size_t c = b % n;
    for (std::size_t i = 0; i < 2 * m; ++i, b = (z * b + t) & MASK, c = b % n)
    {
        result[i] = c;
    }
    return result;
}

value_t* buildPrefixSums(const value_t* a, const std::size_t n)
{
    static const value_t MASK = (1 << 16) - 1;

    value_t* result = new value_t[n];
    value_t sum = a[0];
    for (std::size_t i = 0; i < n; ++i, sum += a[i])
    {
        result[i] = sum;
    }
    return result;
}

unsigned long long solve(const std::size_t n, const value_t x, const value_t y, const value_t a0, const std::size_t m, const std::size_t z, const value_t t, const std::size_t b0)
{
    const value_t* a          = buildData(n, x, y, a0);
    const std::size_t* c      = buildRequests(m, n, z, t, b0);
    const value_t* prefixSums = buildPrefixSums(a, n);

    unsigned long long result = 0;

    for (std::size_t i = 0; i < m; ++i)
    {
        const std::size_t c0 = c[2 * i];
        const std::size_t c1 = c[2 * i + 1];
        const std::size_t l = std::min(c0, c1);
        const std::size_t r = std::max(c0, c1);

        if (l)
        {
            result += prefixSums[r] - prefixSums[l - 1];
        }
        else
        {
            result += prefixSums[r];
        }
    }

    delete[] a;
    delete[] c;
    delete[] prefixSums;

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

    std::size_t n, m, z, b0;
    value_t x, y, a0, t;
    cin >> n >> x >> y >> a0 >> m >> z >> t >> b0;

    std::cout << solve(n, x, y, a0, m, z, t, b0);

    return 0;
}
