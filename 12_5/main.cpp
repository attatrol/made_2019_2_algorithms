/**
  * E. Суффиксный массив
  *
  * ввод стандартный ввод
  * вывод стандартный вывод
  *
  * Постройте суффиксный массив для заданной строки s, для каждых двух соседних суффиксов найдите длину максимального общего префикса.
  *
  * Входные данные
  * Первая строка входного файла содержит строку s (1 ≤ |s| ≤ 400 000).
  * Строка состоит из строчных латинских букв.
  *
  * Выходные данные
  * В первой строке выведите |s| различных чисел — номера первых символов суффиксов строки s так,
  * чтобы соответствующие суффиксы были упорядочены в лексикографически возрастающем порядке.
  * Во второй строке выведите |s| - 1 чисел — длины наибольших общих префиксов.
  */

#define DEBUG

#include <assert.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#ifdef DEBUG
#include <fstream>
#endif
#include <tuple>
#include <queue>
#include <vector>

template <int TAlphabetSize, char TAlphabetOffset>
class SuffixTree
{
private:
    struct Node {
        std::size_t left_;
        std::size_t right_;
        std::size_t parent_;
        std::size_t suffixLink_;
        std::size_t children_[TAlphabetSize];
        std::size_t suffixLength_;

        Node (std::size_t left, std::size_t right, std::size_t parent, std::size_t suffixLink)
            : left_(left), right_(right), parent_(parent), suffixLink_(suffixLink)
        {
            std::fill(children_, children_ + TAlphabetSize, std::numeric_limits<std::size_t>::max());
        }
    };

    std::vector<char> sequence_;
    const std::size_t sizeLimit_;
    std::vector<Node> nodes_;
    std::size_t currentNodeIndex_;
    std::size_t currentRemainder_;

    void traverseStep(std::size_t& nodeIndex, std::size_t& remainder, std::size_t left, std::size_t right)
    {
        while (left < right)
        {
            Node& node = nodes_[nodeIndex];
            const std::size_t edgeLength = node.right_ - node.left_;
            if (remainder == edgeLength)
            {
                remainder = 0;
                if (node.children_[sequence_[left]] == std::numeric_limits<std::size_t>::max())
                {
                    nodeIndex = sizeLimit_ + 1;
                    return;
                }
                else
                {
                    nodeIndex = node.children_[sequence_[left]];
                }
            }
            else if (sequence_[left] != sequence_[nodes_[nodeIndex].left_ + remainder])
            {
                nodeIndex = sizeLimit_ + 1;
                remainder = sizeLimit_ + 1;
                return;
            }
            else if (edgeLength > right - left + remainder)
            {
                remainder = right - left + remainder;
                return;
            }
            else
            {
                left += edgeLength - remainder;
                remainder = edgeLength;
            }
        }
    }
    std::size_t findNode(const std::size_t nodeIndex, const std::size_t remainder)
    {
        Node node = nodes_[nodeIndex];
        if (remainder == node.right_ - node.left_) // this node
        {
            return nodeIndex;
        }
        if (!remainder) // parent
        {
            return node.parent_;
        }
        // insert
        std::size_t newNodeIndex = nodes_.size();
        nodes_.emplace_back(node.left_, node.left_ + remainder, node.parent_, sizeLimit_ + 1);
        nodes_[node.parent_].children_[sequence_[node.left_]] = newNodeIndex;
        nodes_[newNodeIndex].children_[sequence_[node.left_ + remainder]] = nodeIndex;
        nodes_[nodeIndex].parent_ = newNodeIndex;
        nodes_[nodeIndex].left_ += remainder;
        return newNodeIndex;
    }
    std::size_t getSuffixLink(std::size_t nodeIndex)
    {
        if (nodes_[nodeIndex].suffixLink_ != sizeLimit_ + 1)
        {
            return nodes_[nodeIndex].suffixLink_;
        }
        const std::size_t parentSuffixNodeIndex = getSuffixLink(nodes_[nodeIndex].parent_);
        const std::size_t suffixLeft = nodes_[nodeIndex].parent_ ? nodes_[nodeIndex].left_ : nodes_[nodeIndex].left_ + 1;
        std::size_t suffixNodeIndex = parentSuffixNodeIndex;
        std::size_t suffixRemainder = nodes_[parentSuffixNodeIndex].right_ - nodes_[parentSuffixNodeIndex].left_;
        traverseStep(suffixNodeIndex, suffixRemainder, suffixLeft, nodes_[nodeIndex].right_);
        return nodes_[nodeIndex].suffixLink_ = findNode(suffixNodeIndex, suffixRemainder);
    }

    void calcSuffixLengths(std::size_t nodeIndex, std::size_t suffixLength)
    {
        Node& node = nodes_[nodeIndex];
        suffixLength += std::min(sequence_.size(), node.right_) - node.left_;
        node.suffixLength_ = suffixLength;
        if (node.right_ != sizeLimit_)
        {
            for(std::size_t childIndex : node.children_)
            {
                if (childIndex != std::numeric_limits<std::size_t>::max())
                {
                    calcSuffixLengths(childIndex, suffixLength);
                }
            }
        }
    }
    void getSuffixArrayNodes(std::size_t nodeIndex, std::vector<std::size_t>& nodes)
    {
        const Node& node = nodes_[nodeIndex];
        if (node.right_ == sizeLimit_)
        {
            nodes.push_back(nodeIndex);
        }
        else
        {
            for(std::size_t childIndex : node.children_)
            {
                if (childIndex != std::numeric_limits<std::size_t>::max())
                {
                    getSuffixArrayNodes(childIndex, nodes);
                }
            }
        }
    }
    void calcDepths(std::queue<std::size_t>& queue, std::size_t currentDepth, std::vector<std::size_t>& depths)
    {
        assert(queue.front() != nodes_.size());
        queue.push(nodes_.size());
        while(!queue.empty())
        {
            const std::size_t front = queue.front();
            queue.pop();
            if (front == nodes_.size())
            {
                if (!queue.empty())
                {
                    calcDepths(queue, currentDepth + 1, depths);
                }
                return;
            }
            depths[front] = currentDepth;
            for (auto childIndex : nodes_[front].children_)
            {
                if (childIndex != std::numeric_limits<std::size_t>::max())
                {
                    queue.push(childIndex);
                }
            }
        }
    }
    std::tuple<std::vector<std::size_t>, std::vector<std::size_t>, std::size_t> prepareLCA()
    {
       const std::size_t size = nodes_.size();
       const std::size_t maxLogDepth = std::ceil(std::log2(size));
       std::vector<std::size_t> depths(size);
       std::queue<std::size_t> queue;
       queue.push(0);
       calcDepths(queue, 0, depths);

       std::vector<std::size_t> jumps(size * maxLogDepth);

       jumps[0] = 0;
       for (std::size_t i = 1; i < size; ++i)
       {
           jumps[i] = nodes_[i].parent_;
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

       return std::make_tuple(depths, jumps, maxLogDepth);
    }
    std::size_t LCA(std::size_t u, std::size_t v, const std::vector<std::size_t>& depths, const std::vector<std::size_t>& jumps, const std::size_t size, const std::size_t maxLogDepth)
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
        return nodes_[v].parent_;
    }
public:
    SuffixTree(std::size_t sizeLimit) : sizeLimit_(sizeLimit), currentNodeIndex_(0), currentRemainder_(0)
    {
        nodes_.emplace_back(0, 0, sizeLimit_ + 1, 0);
    }
    void append(char value)
    {
        assert(sequence_.size() < sizeLimit_);
        sequence_.push_back(value - TAlphabetOffset);
        const std::size_t newLeft = sequence_.size() - 1;
        const std::size_t newRight = newLeft + 1;
        std::size_t nodeIndex;
        do
        {
            std::size_t currentNodeIndex = currentNodeIndex_;
            std::size_t currentRemainder = currentRemainder_;
            traverseStep(currentNodeIndex, currentRemainder, newLeft, newRight);
            if (currentNodeIndex != sizeLimit_ + 1)
            {
                currentNodeIndex_ = currentNodeIndex;
                currentRemainder_ = currentRemainder;
                break;
            }
            nodeIndex = findNode(currentNodeIndex_, currentRemainder_);
            const std::size_t newNodeIndex = nodes_.size();
            nodes_.emplace_back(newLeft, sizeLimit_, nodeIndex, sizeLimit_ + 1);
            nodes_[nodeIndex].children_[sequence_[newLeft]] = newNodeIndex;
            currentNodeIndex_ = getSuffixLink(nodeIndex);
            currentRemainder_ = nodes_[currentNodeIndex_].right_ - nodes_[currentNodeIndex_].left_;
        }
        while(nodeIndex);
    }
    std::pair<std::vector<std::size_t>, std::vector<std::size_t>> buildSuffixArrayAndLCP()
    {
        calcSuffixLengths(0, 0);
        std::vector<std::size_t> nodes;
        getSuffixArrayNodes(0, nodes);
        const std::size_t size = nodes.size();
        std::vector<std::size_t> suffixArray(size);
        for (std::size_t i = 0; i < size; ++i)
        {
            suffixArray[i] = sequence_.size() - nodes_[nodes[i]].suffixLength_;
        }

        auto [depths, jumps, maxLogDepth] = prepareLCA();
        std::vector<std::size_t> lcp(size - 1);
        for (std::size_t i = 0; i < size - 1; ++i)
        {
            const std::size_t lcaIndex = LCA(nodes[i], nodes[i + 1], depths, jumps, nodes_.size(), maxLogDepth);
            lcp[i] = nodes_[lcaIndex].suffixLength_;
        }
        return std::make_pair(suffixArray, lcp);
    }
};

static const std::size_t MAX_CAPACITY = 400002;

int main()
{
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    std::cout.tie(nullptr);
#endif
    std::string input;
    cin >> input;

    SuffixTree<27, 'a' - 1> tree(MAX_CAPACITY);
    for (char ch : input)
    {
        tree.append(ch);
    }
    tree.append('a' - 1);

    auto [suffixArrray, lcp] = tree.buildSuffixArrayAndLCP();
    for (auto it = suffixArrray.begin() + 1; it < suffixArrray.end(); ++it)
    {
        std::cout << *it + 1 << ' ';
    }
    std::cout << '\n';
    for (auto it = lcp.begin() + 1; it < lcp.end(); ++it)
    {
        std::cout << *it << ' ';
    }

    return 0;
}
