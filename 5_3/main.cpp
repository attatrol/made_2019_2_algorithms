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
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <limits>

using value_t = long long;

static const value_t MIN_NEUTRAL_ELEMENT = std::numeric_limits<value_t>::max();

struct TreeNode
{
    value_t value;
    value_t delayedSummand;
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

std::pair<TreeNode*, std::size_t> buildSegmentTree(const value_t* data, const std::size_t dataSize)
{
    std::size_t dataSizePow2 = getPow2Ceil(dataSize);
    const std::size_t treeSize = dataSizePow2 * 2 - 1;
    TreeNode* result = new TreeNode[treeSize];

    for (std::size_t i = 0; i < dataSize; ++i)
    {
        result[dataSizePow2 + i - 1].value = data[i];
        result[dataSizePow2 + i - 1].delayedSummand = 0;
        result[dataSizePow2 + i - 1].set = false;
    }
    for (std::size_t i = dataSize; i < dataSizePow2; ++i)
    {
        result[dataSizePow2 + i - 1].value = MIN_NEUTRAL_ELEMENT;
        result[dataSizePow2 + i - 1].delayedSummand = 0;
        result[dataSizePow2 + i - 1].set = false;
    }
    for (std::size_t i = dataSizePow2 - 2; i < dataSizePow2; --i)
    {
        result[i].value = std::min(result[2 * i + 1].value, result[2 * i + 2].value);
        result[i].delayedSummand = 0;
        result[i].set = false;
    }

    return std::make_pair(result, treeSize);
}

value_t get(TreeNode* tree, std::size_t index, std::size_t l, std::size_t r)
{
    assert(l <= r);
    if (tree[index].set)
    {
        return tree[index].delayedSummand;
    }
    else if (tree[index].delayedSummand)
    {
        return tree[index].delayedSummand + tree[index].value;
    }
    else
    {
        return tree[index].value;
    }
}

void push(TreeNode* tree, std::size_t index, std::size_t l, std::size_t r)
{
    assert(l <= r);
    if (l == r)
    {
        if (tree[index].set)
        {
            tree[index].value = tree[index].delayedSummand;
            tree[index].set = false;
        }
        else
        {
            tree[index].value += tree[index].delayedSummand;
        }
        tree[index].delayedSummand = 0;
    }
    else
    {
        if (tree[index].set)
        {
            tree[2 * index + 1].delayedSummand = tree[2 * index + 2].delayedSummand = tree[index].delayedSummand;
            tree[2 * index + 1].set = tree[2 * index + 2].set = true;
        }
        else
        {
            tree[2 * index + 1].delayedSummand += tree[index].delayedSummand;
            tree[2 * index + 2].delayedSummand += tree[index].delayedSummand;
        }
        std::size_t m = (l + r) / 2;
        tree[index].delayedSummand = 0;
        tree[index].set = false;
        tree[index].value = std::min(get(tree, 2 * index + 1, l, m), get(tree, 2 * index + 2, m + 1, r));
    }
}

void update(TreeNode* tree, std::size_t index, std::size_t l, std::size_t r, std::size_t a, std::size_t b, value_t value, bool set)
{
    assert(l <= r);
    assert(a <= b);
    if (tree[index].delayedSummand || tree[index].set)
    {
        push(tree, index, l, r);
    }
    if (l > b || r < a)
    {
        return;
    }
    if (l >= a && r <= b)
    {
        tree[index].delayedSummand = value;
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
    if (tree[index].delayedSummand || tree[index].set)
    {
        push(tree, index, l, r);
    }
    if (l > b || r < a) return MIN_NEUTRAL_ELEMENT;
    if (l >= a && r <= b) return get(tree, index, l, r);
    std::size_t m = (l + r) / 2;
    return std::min(rmq(tree, size, 2 * index + 1, l, m, a, b), rmq(tree, size, 2 * index + 2, m + 1, r, a, b));
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

    std::size_t dataSize;
    cin >> dataSize;
    value_t* data = new value_t[dataSize];
    for (std::size_t i = 0; i < dataSize; ++i)
    {
        cin >> data[i];
    }

    auto[tree, treeSize] = buildSegmentTree(data, dataSize);
    std::size_t rRoot = (treeSize + 1) / 2;

    std::string opType;
    while(cin >> opType)
    {
        std::size_t a, b;
        cin >> a >> b;
        switch(opType[0])
        {
        case 's': // set
            value_t value;
            cin >> value;
            update(tree, 0, 1, rRoot, a, b, value, true);
            break;
        case 'a': // add
            value_t summand;
            cin >> summand;
            update(tree, 0, 1, rRoot, a, b, summand, false);
            break;
        case 'm': // min
            std::cout << rmq(tree, treeSize, 0, 1, rRoot, a, b) << '\n';
            break;
        default:
            assert(false);
        }
    }

    delete[] tree;
    delete[] data;

    return 0;
}
