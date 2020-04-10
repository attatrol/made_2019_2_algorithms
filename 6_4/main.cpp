/**
 * D. LCA
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дано подвешенное дерево с корнем в первой вершине.
 * Вам нужно ответить на m запросов вида "найти LCA двух вершин".
 * LCA вершин u и v в подвешенном дереве — это наиболее удалённая от корня дерева вершина, лежащая на обоих путях от u и v до корня.
 *
 * Входные данные
 * В первой строке задано целое число n — число вершин в дереве (1≤n≤2⋅10^5).
 * В следующих n−1 строках записано одно целое число x. Число x на строке i означает, что x — предок вершины i(x<i).
 * Затем дано число m.
 * Далее заданы m (0≤m≤5⋅10^5) запросов вида (u,v) — найти LCA двух вершин u и v (1≤u,v≤n; u≠v).
 *
 * Выходные данные
 * Для каждого запроса выведите LCA двух вершин на отдельной строке.
 */


#define DEBUG

#include <assert.h>
#include <cmath>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
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

std::pair<std::size_t*, std::size_t*> preprocess(const std::vector<std::vector<std::size_t>>& tree, const std::size_t* parents, const std::size_t maxLogDepth)
{
   const std::size_t size = tree.size();
   std::size_t* depths = new std::size_t[size];
   std::queue<std::size_t> queue;
   queue.push(0);
   calcDepths(tree, queue, 0, depths);

   std::size_t* jumps = new std::size_t[size * maxLogDepth];

   for (std::size_t i = 0; i < maxLogDepth; ++i)
   {
       for (std::size_t j = 0; j < size; ++j)
       {
           jumps[j + size * i] = 888;
       }
   }

   for (std::size_t i = 0; i < size; ++i)
   {
       jumps[i] = parents[i];
   }

   for (std::size_t i = 1; i < maxLogDepth; ++i)
   {
       for (std::size_t j = 0; j < size; ++j)
       {
           const std::size_t halfJump = jumps[j + size * (i - 1)];
           assert(halfJump < size);
           const std::size_t coord = halfJump + size * (i - 1);
           assert(coord < size * maxLogDepth);
           jumps[j + size * i] = jumps[coord];
       }
   }

//   for (std::size_t i = 0; i < maxLogDepth; ++i)
//   {
//       for (std::size_t j = 0; j < size; ++j)
//       {
//           std::cout << jumps[j + size * i] << ' ';
//       }
//       std::cout << '\n';
//   }

   return std::make_pair(depths, jumps);
}

std::size_t lca(std::size_t u, std::size_t v, const std::size_t* parents, const std::size_t* depths, const std::size_t* jumps, const std::size_t size, const std::size_t maxLogDepth)
{
    if (depths[v] > depths[u])
    {
        std::swap(u, v);
    }
    for (std::size_t i = maxLogDepth - 1; i < maxLogDepth; --i)
    {
        assert(u + size * i < size * maxLogDepth);
        const std::size_t jumpU = jumps[u + size * i];
        if (depths[jumpU] >= depths[v])
        {
            u = jumpU;
        }
    }
    if (u == v)
    {
        return v;
    }
    for (std::size_t i = maxLogDepth - 1; i < maxLogDepth; --i)
    {
        assert(u + size * i < size * maxLogDepth);
        const std::size_t jumpU = jumps[u + size * i];
        assert(v + size * i < size * maxLogDepth);
        const std::size_t jumpV = jumps[v + size * i];
        if (jumpU != jumpV)
        {
            u = jumpU;
            v = jumpV;
        }
    }
    return parents[v];
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

    std::size_t size;
    cin >> size;

    std::vector<std::vector<std::size_t>> tree(size);
    std::size_t* parents = new std::size_t[size];
    parents[0] = 0;

    for (std::size_t i = 1; i < size; ++i)
    {
        std::size_t parent;
        cin >> parent;
        tree[--parent].push_back(i);
        parents[i] = parent;
    }

    std::size_t maxLogDepth = std::ceil(std::log2(size));
    auto [depths, jumps] = preprocess(tree, parents, maxLogDepth);

    std::size_t opCount;
    cin >> opCount;

    for (std::size_t i = 0; i < opCount; ++i)
    {
        std::size_t u, v;
        cin >> u >> v;
        std::cout << lca(u - 1, v - 1, parents, depths, jumps, size, maxLogDepth) + 1 << '\n';
    }

    delete[] depths;
    delete[] jumps;
    delete[] parents;

    return 0;
}
