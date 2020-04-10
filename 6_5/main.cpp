/**
 * E. Самое дешевое ребро
 *
 * ввод minonpath.in
 * вывод minonpath.out
 *
 * Дано подвешенное дерево с корнем в первой вершине.
 * Все ребра имеют веса (стоимости).
 * ам нужно ответить на M запросов вида "найти у двух вершин минимум среди стоимостей ребер пути между ними".
 *
 * Входные данные
 * В первой строке задано целое число n — число вершин в дереве (1 ≤ n ≤ 2·10^5).
 * В следующих n - 1 строках записаны два целых числа x и y.
 * Число x на строке i означает, что x — предок вершины i, y задает стоимость ребра (x < i; |y| ≤ 10^6).
 * Далее заданы m (0 ≤ m ≤ 5·10^5) запросов вида (x, y) — найти минимум на пути из x в y (1 ≤ x, y ≤ n; x ≠ y).
 *
 * Выходные данные
 * Выведите ответы на запросы.
 */


//#define DEBUG

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <fstream>
#ifdef DEBUG
#include <iostream>
#endif
#include <limits>
#include <tuple>
#include <queue>
#include <vector>


void calcDepths(const std::vector<std::vector<std::size_t>>& tree, std::queue<std::size_t>& queue, std::size_t currentDepth, std::size_t* depths)
{
    assert(queue.front() != tree.size());
    queue.push(tree.size());
    while(!queue.empty())
    {
        const std::size_t front = queue.front();
        queue.pop();
        if (front == tree.size())
        {
            if (!queue.empty())
            {
                calcDepths(tree, queue, currentDepth + 1, depths);
            }
            return;
        }
        depths[front] = currentDepth;
        for (auto it = tree[front].begin(); it < tree[front].end(); ++it)
        {
            queue.push(*it);
        }
    }
}

std::tuple<std::size_t*, std::size_t*, int*> preprocess(const std::vector<std::vector<std::size_t>>& tree, const std::size_t* parents, const int* values, const std::size_t maxLogDepth)
{
   const std::size_t size = tree.size();
   std::size_t* depths = new std::size_t[size];
   std::queue<std::size_t> queue;
   queue.push(0);
   calcDepths(tree, queue, 0, depths);

   std::size_t* jumps = new std::size_t[size * maxLogDepth];
   int* mins = new int[size * maxLogDepth];

   for (std::size_t i = 0; i < size; ++i)
   {
       jumps[i] = parents[i];
       mins[i] = values[i];
   }

   for (std::size_t i = 1; i < maxLogDepth; ++i)
   {
       for (std::size_t j = 0; j < size; ++j)
       {
           const std::size_t halfJump = jumps[j + size * (i - 1)];
           const int min1 = mins[j + size * (i - 1)];
           assert(halfJump < size);
           const std::size_t coord = halfJump + size * (i - 1);
           const int min2 = mins[coord];
           assert(coord < size * maxLogDepth);
           jumps[j + size * i] = jumps[coord];
           mins[j + size * i] = std::min(min1, min2);
       }
   }

   return std::make_tuple(depths, jumps, mins);
}

int getPathMinVertex(std::size_t u, std::size_t v, const std::size_t* depths, const std::size_t* jumps,
                const int* mins, const std::size_t size, const std::size_t maxLogDepth)
{
    if (depths[v] > depths[u])
    {
        std::swap(u, v);
    }

    int min = std::numeric_limits<int>::max();
    for (std::size_t i = maxLogDepth - 1; i < maxLogDepth; --i)
    {
        assert(u + size * i < size * maxLogDepth);
        const std::size_t jumpU = jumps[u + size * i];
        if (depths[jumpU] >= depths[v])
        {
            min = std::min(min, mins[u + size * i]);
            u = jumpU;
        }
    }
    if (u == v)
    {
        return min;
    }
    for (std::size_t i = maxLogDepth - 1; i < maxLogDepth; --i)
    {
        assert(u + size * i < size * maxLogDepth);
        const std::size_t jumpU = jumps[u + size * i];
        assert(v + size * i < size * maxLogDepth);
        const std::size_t jumpV = jumps[v + size * i];
        if (jumpU != jumpV)
        {
            min = std::min({min, mins[u + size * i], mins[v + size * i]});
            u = jumpU;
            v = jumpV;
        }
    }
    return std::min({min, mins[u], mins[v]});
}

int main()
{
#ifdef DEBUG
    std::ifstream cin("input.txt");
    using std::cout;
#else
    std::ifstream cin("minonpath.in");
    std::ofstream cout("minonpath.out");
#endif

    std::size_t size;
    cin >> size;

    std::vector<std::vector<std::size_t>> tree(size);
    std::size_t* parents = new std::size_t[size];
    int* values = new int[size];
    parents[0] = 0;
    values[0] = std::numeric_limits<int>::max();

    for (std::size_t i = 1; i < size; ++i)
    {
        std::size_t parent;
        int value;
        cin >> parent >> value;
        tree[--parent].push_back(i);
        parents[i] = parent;
        values[i] = value;
    }

    std::size_t maxLogDepth = std::ceil(std::log2(size));
    auto [depths, jumps, mins] = preprocess(tree, parents, values, maxLogDepth);

    std::size_t opCount;
    cin >> opCount;

    for (std::size_t i = 0; i < opCount; ++i)
    {
        std::size_t u, v;
        cin >> u >> v;
        cout << getPathMinVertex(u - 1, v - 1, depths, jumps, mins, size, maxLogDepth) << '\n';
    }

    delete[] depths;
    delete[] jumps;
    delete[] parents;
    delete[] mins;

    return 0;
}
