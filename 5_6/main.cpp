/**
 * F. RMQ наоборот
 *
 * ввод rmq.in
 * вывод rmq.out
 *
 * Рассмотрим массив a[1..n].
 * Пусть Q(i, j) — ответ на запрос о нахождении минимума среди чисел a[i], ..., a[j].
 * Вам даны несколько запросов и ответы на них.
 * Восстановите исходный массив.
 *
 * Входные данные
 * Первая строка входного файла содержит число n — размер массива, и m — число запросов (1 ≤ n, m ≤ 100 000).
 * Следующие m строк содержат по три целых числа i, j и q, означающих, что Q(i, j) = q (1 ≤ i ≤ j ≤ n,  - 231 ≤ q ≤ 231 - 1).
 *
 * Выходные данные
 * Если искомого массива не существует, выведите строку «inconsistent».
 * В противном случае в первую строку выходного файла выведите «consistent».
 * Во вторую строку выходного файла выведите элементы массива.
 * Элементами массива должны быть целые числа в интервале от  - 231 до 231 - 1 включительно.
 * Если решений несколько, выведите любое.
 */

/**
 * C. RMQ2
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Входные данные
 * В первой строке находится число n — размер массива. (1 ≤ n ≤ 10^5).
 * Во второй строке находится n чисел ai — элементы массива.
 * Далее содержится описание операций, их количество не превышает 2·10^5.
 * В каждой строке находится одна из следующих операций:
 * set i j x — установить все a[k], i ≤ k ≤ j в x.
 * add i j x — увеличить все a[k], i ≤ k ≤ j на x.
 * min i j — вывести значение минимального элемента в массиве на отрезке с i по j, гарантируется, что (1 ≤ i ≤ j ≤ n).
 * Все числа во входном файле и результаты выполнения всех операций не превышают по модулю 10^18.
 *
 * Выходные данные
 * Выведите последовательно результат выполнения всех операций min. Следуйте формату выходного файла из примера.
 *
 * Пример:
 *
 * Входные данные
 * 5
 * 1 2 3 4 5
 * min 2 5
 * min 1 5
 * min 1 4
 * min 2 4
 * set 1 3 10
 * add 2 4 4
 * min 2 5
 * min 1 5
 * min 1 4
 * min 2 4
 *
 * Выходные данные
 * 2
 * 1
 * 1
 * 2
 * 5
 * 5
 * 8
 * 8
 */

#define DEBUG

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <fstream>
#ifdef DEBUG
#include <iostream>
#endif
#include <limits>

using value_t = int;

static const value_t MIN_NEUTRAL_ELEMENT = std::numeric_limits<value_t>::max();

struct TreeNode
{
    value_t value;
    value_t delayedValue;
    bool set;
};

inline std::size_t getPow2Ceil(std::size_t n)
{
    std::size_t result = 1;
    while(result < n)
    {
        result <<= 1;
    }
    return result;
}

std::pair<TreeNode*, std::size_t> buildSegmentTree(const std::size_t dataSize)
{
    std::size_t dataSizePow2 = getPow2Ceil(dataSize);
    const std::size_t treeSize = dataSizePow2 * 2 - 1;
    TreeNode* result = new TreeNode[treeSize];

    for (std::size_t i = 0; i < dataSizePow2; ++i)
    {
        result[dataSizePow2 + i - 1].value = MIN_NEUTRAL_ELEMENT;
        result[dataSizePow2 + i - 1].delayedValue = 0;
        result[dataSizePow2 + i - 1].set = false;
    }

    return std::make_pair(result, treeSize);
}

value_t get(TreeNode* tree, std::size_t index, std::size_t l, std::size_t r)
{
    assert(l <= r);
    if (tree[index].set)
    {
        return tree[index].delayedValue;
    }
    else
    {
        return tree[index].delayedValue + tree[index].value;
    }
}

void push(TreeNode* tree, std::size_t index, std::size_t l, std::size_t r)
{
    assert(l <= r);
    if (l == r)
    {
        if (tree[index].set)
        {
            tree[index].value = tree[index].delayedValue;
            tree[index].set = false;
        }
        else
        {
            tree[index].value += tree[index].delayedValue;
        }
        tree[index].delayedValue = 0;
    }
    else
    {
        if (tree[index].set)
        {
            tree[2 * index + 1].delayedValue = tree[2 * index + 2].delayedValue = tree[index].delayedValue;
            tree[2 * index + 1].set = tree[2 * index + 2].set = true;
        }
        else
        {
            tree[2 * index + 1].delayedValue += tree[index].delayedValue;
            tree[2 * index + 2].delayedValue += tree[index].delayedValue;
        }
        std::size_t m = (l + r) / 2;
        tree[index].delayedValue = 0;
        tree[index].set = false;
        tree[index].value = std::min(get(tree, 2 * index + 1, l, m), get(tree, 2 * index + 2, m + 1, r));
    }
}

void update(TreeNode* tree, std::size_t index, std::size_t l, std::size_t r, std::size_t a, std::size_t b, value_t value, bool set)
{
    assert(l <= r);
    assert(a <= b);
    if (tree[index].delayedValue || tree[index].set)
    {
        push(tree, index, l, r);
    }
    if (l > b || r < a)
    {
        return;
    }
    if (l >= a && r <= b)
    {
        tree[index].delayedValue = value;
        tree[index].set = set;
        return;
    }
    std::size_t m = (l + r) / 2;
    update(tree, 2 * index + 1, l, m, a, b, value, set);
    update(tree, 2 * index + 2, m + 1, r, a, b, value, set);
    tree[index].value = std::min(get(tree, 2 * index + 1, l, m), get(tree, 2 * index + 2, m + 1, r));
}

value_t rmq(TreeNode* tree, const std::size_t size, std::size_t index, std::size_t l, std::size_t r, std::size_t a, std::size_t b)
{
    assert(l <= r);
    assert(a <= b);
    assert(index < size);
    if (tree[index].delayedValue || tree[index].set)
    {
        push(tree, index, l, r);
    }
    if (l > b || r < a) return MIN_NEUTRAL_ELEMENT;
    if (l >= a && r <= b) return get(tree, index, l, r);
    std::size_t m = (l + r) / 2;
    return std::min(rmq(tree, size, 2 * index + 1, l, m, a, b), rmq(tree, size, 2 * index + 2, m + 1, r, a, b));
}

struct Query
{
    std::size_t a;
    std::size_t b;
    value_t value;
};

int main()
{
#ifdef DEBUG
    std::ifstream cin("input.txt");
    using std::cout;
#else
     std::ifstream cin("rmq.in");
     std::ofstream cout("rmq.out");
#endif

    std::size_t dataSize, opCount;
    cin >> dataSize >> opCount;

    auto[tree, treeSize] = buildSegmentTree(dataSize);
    std::size_t rRoot = (treeSize + 1) / 2;

    Query* queries = new Query[opCount];

    for (std::size_t i = 0; i < opCount; ++i)
    {
        cin >> queries[i].a >> queries[i].b >> queries[i].value;
    }

    std::sort(queries, queries + opCount, [](const Query& q1, const Query& q2)
    {
      return q1.value < q2.value;
    });

    for (std::size_t i = 0; i < opCount; ++i)
    {
        update(tree, 0, 1, rRoot, queries[i].a, queries[i].b, queries[i].value, true);
    }

    bool consistent = true;
    for (std::size_t i = 0; i < opCount; ++i)
    {
        if (queries[i].value != rmq(tree, treeSize, 0, 1, rRoot, queries[i].a, queries[i].b))
        {
            cout << "inconsistent";
            consistent = false;
            break;

        }
    }
    if (consistent)
    {
        cout << "consistent\n";
        for (std::size_t i = 1; i <= dataSize; ++i)
        {
            cout << rmq(tree, treeSize, 0, 1, rRoot, i, i) << ' ';
        }
    }

    delete[] queries;
    delete[] tree;

    return 0;
}
