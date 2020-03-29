/**
 * D. Продавец аквариумов
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Продавец аквариумов для кошек хочет объехать n городов, посетив каждый из них ровно один раз.
 * Помогите ему найти кратчайший путь.
 *
 * Входные данные
 * Первая строка входного файла содержит натуральное число n (1 ≤ n ≤ 13) — количество городов.
 * Следующие n строк содержат по n чисел — длины путей между городами.
 * В i-й строке j-е число — ai, j — это расстояние между городами i и j (0 ≤ ai, j ≤ 106; ai, j = aj, i; ai, i = 0).
 *
 * Выходные данные
 * В первой строке выходного файла выведите длину кратчайшего пути. Во второй строке выведите n чисел — порядок, в котором нужно посетить города.
 */

//#define DEBUG

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <limits>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>

std::pair<int, std::vector<int>> solveTravellingSalesman(const std::vector<int>& distanceMatrix, const std::size_t vertexCount)
{
    if (vertexCount == 1)
    {
        return std::make_pair(0, std::vector<int>{0});
    }

    static const int UNREACHABLE_VALUE = std::numeric_limits<int>::max();

    // from graphics.stanford.edu/~seander/bithacks.html
    std::size_t BitsSetTable256[256];
    BitsSetTable256[0] = 0;
    for (std::size_t i = 0; i < 256; i++)
    {
      BitsSetTable256[i] = (i & 1) + BitsSetTable256[i / 2];
    }

    int* dp = new int[vertexCount * (1 << vertexCount)];
    int* returns = new int[vertexCount * (1 << vertexCount)];
#ifdef DEBUG
    std::fill(dp, dp + vertexCount * (1 << vertexCount), UNREACHABLE_VALUE);
#endif

    for (std::size_t i = 0; i < vertexCount; ++i)
    {
        for (std::size_t j = i + 1; j < vertexCount; ++j)
        {
            assert(vertexCount * (1 << i) + j < vertexCount * (1 << vertexCount));
            assert(vertexCount * (1 << j) + i < vertexCount * (1 << vertexCount));
            dp[vertexCount * (1 << i) + j] = dp[vertexCount * (1 << j) + i] = distanceMatrix[i * vertexCount + j];
        }
    }

    for (std::size_t len = 2; len < vertexCount; ++ len)
    {
        for (std::size_t mask = 0; mask < 1 << vertexCount; ++ mask)
        {
            if (BitsSetTable256[mask & 0xff] + BitsSetTable256[(mask >> 8) & 0xff] == len)
            {
                for (std::size_t end = 0; end < vertexCount; ++end)
                {
                    const std::size_t coord = end + vertexCount * mask;
                    assert(coord < vertexCount * (1 << vertexCount));
#ifdef DEBUG
                    int cost = dp[coord];
                    assert(cost == UNREACHABLE_VALUE);
#else
                    int cost = UNREACHABLE_VALUE;
#endif
                    if (!((1 << end) & mask))
                    {
                        for (std::size_t prev = 0; prev < vertexCount; ++prev)
                        {
                            if ((1 << prev) & mask)
                            {
                                const std::size_t prevCoord = prev + vertexCount * (mask - (1 << prev));
                                assert(prevCoord < vertexCount * (1 << vertexCount));
                                assert(vertexCount * prev + end < vertexCount * vertexCount);
                                const int newCost = distanceMatrix[vertexCount * prev + end] + dp[prevCoord];
                                if (newCost < cost)
                                {
                                    cost = dp[coord] = newCost;
                                    returns[coord] = static_cast<int>(prev);
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    int minEnd;
    int minCost = UNREACHABLE_VALUE;
    for (std::size_t i = 0; i < vertexCount; ++i)
    {
        const std::size_t coord = ((1 << vertexCount) - 1 - (1 << i)) * vertexCount + i;
#ifdef DEBUG
        assert(dp[coord] != UNREACHABLE_VALUE);
#endif
        if (dp[coord] < minCost)
        {
            minCost = dp[coord];
            minEnd = static_cast<int>(i);
        }
    }

    std::vector<int> path(vertexCount);
    path[0] = minEnd;
    std::size_t mask = (1 << vertexCount) - 1 - (1 << minEnd);
    for (std::size_t i = 1; i < vertexCount - 1; ++i)
    {
        std::size_t coord = mask * vertexCount + minEnd;
        assert(coord < vertexCount * (1 << vertexCount));
        minEnd = path[i] = returns[coord];
        assert(mask & (1 << minEnd));
        mask -= 1 << minEnd;
    }
    assert(BitsSetTable256[mask & 0xff] + BitsSetTable256[(mask >> 8) & 0xff] == 1);
    minEnd = 0;
    for (std::size_t i = 1; mask; ++i)
    {
        ++minEnd;
        mask >>= 1;
    }
    path[vertexCount - 1] = minEnd - 1;

    delete[] dp;
    delete[] returns;

    return std::make_pair(minCost, path);
}

int main()
{
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
#endif

    std::size_t vertexCount;
    cin >> vertexCount;

    std::vector<int> distanceMatrix(vertexCount * vertexCount);

    for (std::size_t i = 0; i < vertexCount; ++i)
    {
        for (std::size_t j = 0; j < vertexCount; ++j)
        {
            cin >> distanceMatrix[i * vertexCount + j];
            assert(i < j || distanceMatrix[i * vertexCount + j] == distanceMatrix[j * vertexCount + i]);
        }
    }

    auto result = solveTravellingSalesman(distanceMatrix, vertexCount);

    std::cout << result.first << '\n';
    for (const int& index : result.second)
    {
        std::cout << (index + 1) << ' ';
    }

    return 0;
}
