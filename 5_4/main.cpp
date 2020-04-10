/**
 * D. Разреженные таблицы
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 * Дан массив из n чисел.
 * Требуется написать программу, которая будет отвечать на запросы следующего вида:
 * найти минимум на отрезке между u и v включительно.
 *
 * Входные данные
 * В первой строке зданы три натуральных числа n, m (1⩽n⩽10^5, 1⩽m⩽10^7) и a1 (0⩽a1<16714589) — количество элементов в массиве, количество запросов и первый элемент массива соответственно.
 * Вторая строка содержит два натуральных числа u1 и v1 (1⩽u1,v1⩽n) — первый запрос.
 *
 * Для того, размер ввода был небольшой, массив и запросы генерируются.
 * Элементы a2,a3,…,an задаются следующей формулой: a_{i+1}=(23⋅a_i+21563) mod 16714589.
 * Например, при n=10, a1=12345 получается следующий массив: a = (12345, 305498, 7048017, 11694653, 1565158, 2591019, 9471233, 570265, 13137658, 1325095).
 * Запросы генерируются следующим образом:
 * u_{i+1} = ((17⋅ui+751+ri+2i) mod n)+1, v_{i+1} = ((13⋅vi+593+ri+5i) mod n)+1, где ri — ответ на запрос номер i.
 * Обратите внимание, что ui может быть больше, чем vi.
 *
 * Выходные данные
 * В выходной файл выведите um, vm и rm (последний запрос и ответ на него).
 */

#define DEBUG

#include <assert.h>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#include <limits>
#endif
#include <tuple>
#include <vector>

using value_t = unsigned long long;

std::vector<value_t> generateData(value_t a, const std::size_t n)
{
    static const value_t MUL_A = 23;
    static const value_t SUM_A = 21563;
    static const value_t MOD_A = 16714589;

    std::vector<value_t> result(n);
    for (std::size_t i = 0; i < n; ++i, a = (MUL_A * a + SUM_A) % MOD_A)
    {
        result[i] = a;
    }

    return result;
}

inline std::size_t getPow2Floor(value_t n)
{
    std::size_t result = 0;
    while (n)
    {
        n >>= 1;
        ++result;
    }
    return result;
}

std::vector<value_t> generateSparceTable(const std::vector<value_t>& data, const std::size_t n, const std::size_t pow2)
{
#ifdef DEBUG
    std::vector<value_t> result(n * pow2, std::numeric_limits<value_t>::max());
#else
    std::vector<value_t> result(n * pow2);
#endif

    for (std::size_t i = 0; i < n; ++i)
    {
        result[i] = data[i];
    }

    for (std::size_t i = 1; i < pow2; ++i)
    {
        for (std::size_t j = 0; j <= n - (1 << i); ++j)
        {
            const std::size_t coord = i * n + j;
            assert(coord < n * pow2);
            const std::size_t parent0Coord = (i - 1) * n + j;
            assert(parent0Coord < n * i && parent0Coord >= n * (i - 1));
            const std::size_t parent1Coord = (i - 1) * n + j + (1 << (i - 1));
            assert(parent1Coord < n * i && parent1Coord >= n * (i - 1));
            result[coord] = std::min(result[parent0Coord], result[parent1Coord]);
        }
    }

    return result;
}

inline value_t rmq(const std::vector<value_t>& sparceTable, const std::size_t n, const std::size_t l, const std::size_t r)
{
    const std::size_t size = r - l + 1;
    const std::size_t pow2LR = getPow2Floor(size) - 1;
    const std::size_t coord0 = pow2LR * n + l;
    const std::size_t coord1 = pow2LR * n + r - (1 << pow2LR) + 1;
#ifdef DEBUG
    assert(sparceTable[coord0] != std::numeric_limits<value_t>::max() && sparceTable[coord1] != std::numeric_limits<value_t>::max());
#endif
    return std::min(sparceTable[coord0], sparceTable[coord1]);
}

std::tuple<value_t, value_t, value_t> solve(const std::size_t n, const std::size_t m, const value_t a0, const std::size_t u0, const std::size_t v0)
{
    const std::vector<value_t> data = generateData(a0, n);
    const std::size_t pow2 = getPow2Floor(n);
    const std::vector<value_t> sparceTable = generateSparceTable(data, n, pow2);

    static const value_t MUL_U = 17;
    static const value_t SUM_U = 751;
    static const value_t MUL_UI = 2;
    static const value_t MUL_V = 13;
    static const value_t SUM_V = 593;
    static const value_t MUL_VI = 5;

    value_t u = u0;
    value_t v = v0;
    assert(u < n && v < n);
    value_t res;

    {
        const std::size_t l = std::min(u, v);
        const std::size_t r = std::max(u, v);
        res = rmq(sparceTable, n, l, r);
#ifdef DEBUG
        std::cout << "1 " << (u + 1) << ' ' << (v + 1) << ' ' << res << '\n';
#endif
    }
    for (std::size_t i = 1; i < m; ++i)
    {
        u = (MUL_U * (u + 1) + SUM_U + res + MUL_UI * i) % n;
        v = (MUL_V * (v + 1) + SUM_V + res + MUL_VI * i) % n;
        const std::size_t l = std::min(u, v);
        const std::size_t r = std::max(u, v);
        res = rmq(sparceTable, n, l, r);
#ifdef DEBUG
        std::cout << (i + 1) << ' ' << (u + 1) << ' ' << (v + 1) << ' ' << res << '\n';
#endif
    }

    return std::make_tuple(u, v, res);
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

    std::size_t n, m, u0, v0;
    value_t a0;
    cin >> n >> m >> a0 >> u0 >> v0;

    auto answer = solve(n, m, a0, u0 - 1, v0 - 1);

    std::cout << (std::get<0>(answer) + 1) << ' ' << (std::get<1>(answer) + 1) << ' ' << std::get<2>(answer);

    return 0;
}


