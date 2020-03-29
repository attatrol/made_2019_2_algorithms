/**
 * C. Выбор вершин дерева
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дан граф, являющийся деревом.
 * Необходимо найти размер наибольшего по мощности независимого множества.
 * Множество вершин графа называется независимым, если никакие две вершины этого множества не соединены ребром.
 *
 * Входные данные
 * Граф в этой задаче задан в виде корневого дерева.
 * В графе выделена вершина — корень дерева.
 * Для каждой вершины i, не являющейся корнем, задан номер вершины-предка pi в корневом дереве.
 * Дерево, заданное таким образом, состоит из рёбер i—pi для всех вершин i, кроме корня.
 * В первой строке входного файла записано целое число n — количество вершин в графе (1 ≤ n ≤ 100).
 * В следующих n строках задан граф.
 * В i-й из этих строк записано целое число pi — номер вершины-предка i-й вершины.
 * Для корня дерева pi = 0; для всех остальных вершин 1 ≤ pi ≤ n.
 * Гарантируется, что заданный во входном файле граф является деревом.
 *
 * Выходные данные
 * В первой строке выходного файла выведите одно число — размер наибольшего по мощности независимого множества.
 */

#define DEBUG

#include <algorithm>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>

struct TreeNode {
    std::vector<std::size_t> children_;
    std::size_t subtreeSetSizeBelongs_;
    std::size_t subtreeSetSizeAbsent_;
};

void buildMaxIndependentSetSize(TreeNode* tree, std::size_t nodeIndex)
{
    TreeNode* node = tree + nodeIndex;
    for (std::size_t childIndex : node->children_)
    {
        buildMaxIndependentSetSize(tree, childIndex);
    }
    std::size_t maxSetSizeSum    = 0;
    std::size_t absentSetSizeSum = 0;
    for (std::size_t childIndex : node->children_)
    {
        const TreeNode* child = tree + childIndex;
        maxSetSizeSum += std::max(child->subtreeSetSizeAbsent_, child->subtreeSetSizeBelongs_);
        absentSetSizeSum += child->subtreeSetSizeAbsent_;
    }
    node->subtreeSetSizeAbsent_ = maxSetSizeSum;
    node->subtreeSetSizeBelongs_ = 1 + absentSetSizeSum;
}

int main()
{
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
#endif

    std::size_t nodeCount;
    cin >> nodeCount;
    std::size_t rootIndex;
    TreeNode* tree = new TreeNode[nodeCount];
    for (std::size_t i = 0; i < nodeCount; ++i)
    {
        std::size_t parentIndex;
        cin >> parentIndex;
        if (!parentIndex)
        {
            rootIndex = i;
        }
        else
        {
            tree[parentIndex - 1].children_.push_back(i);
        }
    }

    buildMaxIndependentSetSize(tree, rootIndex);
    std::cout << std::max(tree[rootIndex].subtreeSetSizeAbsent_, tree[rootIndex].subtreeSetSizeBelongs_);

    delete[] tree;

    return 0;
}
