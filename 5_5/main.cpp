/**
 * E. Криптография
 *
 * ввод
 * crypto.in
 * вывод
 * crypto.out
 *
 * Задано n матриц A1, A2, ..., An размера 2 × 2.
 * Необходимо для нескольких запросов вычислить произведение матриц Ai, Ai + 1, ..., Aj.
 * Все вычисления производятся по модулю r.
 *
 * Входные данные
 * Первая строка входного файла содержит числа r (1 ≤ r ≤ 10 000), n (1 ≤ n ≤ 200 000) и m (1 ≤ m ≤ 200 000).
 * Следующие n блоков по две строки содержащие по два числа в строке — описания матриц.
 * Затем следуют m пар целых чисел от 1 до n, запросы на произведение на отрезке.
 *
 * Выходные данные
 * Выведите m блоков по две строки,по два числа в каждой — произведения на отрезках.
 * Разделяйте блоки пустой строкой.
 * Все вычисления производятся по модулю.
 *
 * Пример
 *
 * Входные данные
 * 3 4 4
 * 0 1
 * 0 0
 *
 * 2 1
 * 1 2
 *
 * 0 0
 * 0 2
 *
 * 1 0
 * 0 2
 *
 * 1 4
 * 2 3
 * 1 3
 * 2 2
 *
 * Выходные данные
 * 0 2
 * 0 0
 *
 * 0 2
 * 0 1
 *
 * 0 1
 * 0 0
 *
 * 2 1
 * 1 2
 */

//#define DEBUG

#include <algorithm>
#include <assert.h>
#include <fstream>
#ifdef DEBUG
#include <iostream>
#endif

using value_t = int;

//// a*b mod mod with regard to overflow
//value_t mulMod(value_t a, value_t b, value_t mod)
//{
//    value_t result = 0;
//    a %= mod;
//    while (b)
//    {
//        if (b & 1)
//        {
//            result = (result + a) % mod;
//        }
//        a = (2 * a) % mod;
//        b >>= 1;
//    }
//    return result;
//}

static const value_t MIN_NEUTRAL_ELEMENT = std::numeric_limits<value_t>::max();

struct TreeNode
{
    value_t a00;
    value_t a01;
    value_t a10;
    value_t a11;

//    TreeNode& operator=(const TreeNode& other)
//    {
//        a00 = other.a00;
//        a01 = other.a01;
//        a10 = other.a10;
//        a11 = other.a11;
//    }
};

static const TreeNode IDENTITY_2X2_MATRIX = { 1, 0, 0, 1};

void multiply2x2Matrices(TreeNode& A, TreeNode& B, TreeNode& result, value_t mod)
{
    result.a00 = (A.a00 * B.a00 + A.a01 * B.a10) % mod;
    result.a01 = (A.a00 * B.a01 + A.a01 * B.a11) % mod;
    result.a10 = (A.a10 * B.a00 + A.a11 * B.a10) % mod;
    result.a11 = (A.a10 * B.a01 + A.a11 * B.a11) % mod;
}

TreeNode multiply2x2Matricesc(TreeNode&& A, TreeNode&& B, value_t mod)
{
    TreeNode result;
    result.a00 = (A.a00 * B.a00 + A.a01 * B.a10) % mod;
    result.a01 = (A.a00 * B.a01 + A.a01 * B.a11) % mod;
    result.a10 = (A.a10 * B.a00 + A.a11 * B.a10) % mod;
    result.a11 = (A.a10 * B.a01 + A.a11 * B.a11) % mod;
    return result;
}


inline std::size_t getPow2Ceil(std::size_t n)
{
    std::size_t result = 1;
    while(result < n)
    {
        result <<= 1;
    }
    return result;
}

std::pair<TreeNode*, std::size_t> buildSegmentTree(const TreeNode* data, const std::size_t dataSize, value_t mod)
{
    std::size_t dataSizePow2 = getPow2Ceil(dataSize);
    const std::size_t treeSize = dataSizePow2 * 2 - 1;
    TreeNode* result = new TreeNode[treeSize];

    for (std::size_t i = 0; i < dataSize; ++i)
    {
        result[dataSizePow2 + i - 1] = data[i];
    }
    for (std::size_t i = dataSize; i < dataSizePow2; ++i)
    {
        result[dataSizePow2 + i - 1] = IDENTITY_2X2_MATRIX;
    }
    for (std::size_t i = dataSizePow2 - 2; i < dataSizePow2; --i)
    {
        multiply2x2Matrices(result[2 * i + 1], result[2 * i + 2], result[i], mod);
    }

    return std::make_pair(result, treeSize);
}

TreeNode rmmq(const TreeNode* tree, const value_t mod, std::size_t index, std::size_t l, std::size_t r, std::size_t a, std::size_t b)
{
    assert(l <= r);
    assert(a <= b);
    if (l > b || r < a) return IDENTITY_2X2_MATRIX;
    if (l >= a && r <= b) return tree[index];
    std::size_t m = (l + r) / 2;
    return multiply2x2Matricesc(rmmq(tree, mod, 2 * index + 1, l, m, a, b), rmmq(tree, mod, 2 * index + 2, m + 1, r, a, b), mod);
}

int main()
{
#ifdef DEBUG
    std::ifstream cin("input.txt");
    using std::cout;
#else
    std::ifstream cin("crypto.in");
    std::ofstream cout("crypto.out");
#endif

    value_t mod;
    std::size_t dataSize, opCount;
    cin >> mod >> dataSize >> opCount;
    TreeNode* data = new TreeNode[dataSize];
    for (std::size_t i = 0; i < dataSize; ++i)
    {
        cin >> data[i].a00 >> data[i].a01 >> data[i].a10 >> data[i].a11;
    }

    auto[tree, treeSize] = buildSegmentTree(data, dataSize, mod);
    std::size_t rRoot = (treeSize + 1) / 2;

    std::string opType;
    for (std::size_t i = 0; i < opCount; ++i)
    {
        std::size_t a, b;
        cin >> a >> b;
        TreeNode answer = rmmq(tree, mod, 0, 1, rRoot, a, b);
        cout << answer.a00 << ' ' << answer.a01 << '\n' << answer.a10 << ' ' << answer.a11 << "\n\n";
    }

    delete[] tree;
    delete[] data;

    return 0;
}
