/**
  * B. Количество подстрок
  *
  * ввод стандартный ввод
  * вывод стандартный вывод
  *
  * Вычислите количество различных подстрок строки s.
  *
  * Входные данные
  * Единственная строка входного файла содержит строку s (1 ≤ |s| ≤ 400 000).
  * Строка состоит из строчных латинских букв.
  *
  * Выходные данные
  * Выведите одно число — ответ на задачу.
  */

#define DEBUG

#include <assert.h>
#include <algorithm>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>
#include <unordered_map>

template <typename T>
class SuffixTree
{
private:
    struct Node {
        std::size_t left_;
        std::size_t right_;
        std::size_t parent_;
        std::size_t suffixLink_;
        std::unordered_map<T, std::size_t> children_;

        Node (std::size_t left, std::size_t right, std::size_t parent, std::size_t suffixLink)
            : left_(left), right_(right), parent_(parent), suffixLink_(suffixLink)
        {
        }
    };

    std::vector<T> sequence_;
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
                auto findResult = node.children_.find(sequence_[left]);
                if (findResult == node.children_.end())
                {
                    nodeIndex = sizeLimit_ + 1;
                    return;
                }
                else
                {
                    nodeIndex = findResult->second;
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

public:
    SuffixTree(std::size_t sizeLimit) : sizeLimit_(sizeLimit), currentNodeIndex_(0), currentRemainder_(0)
    {
        nodes_.emplace_back(0, 0, sizeLimit_ + 1, 0);
    }
    void append(T value)
    {
        assert(sequence_.size() < sizeLimit_);
        sequence_.push_back(value);
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
    unsigned long long countSubstrings()
    {
        unsigned long long  result = 0;
        for (std::size_t i = 1; i < nodes_.size(); ++i)
        {
            Node& node = nodes_[i];
            result += std::min(sequence_.size(), node.right_) - node.left_;
        }
        return result;
    }
};

static const std::size_t MAX_CAPACITY = 400000;

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
    SuffixTree<char> tree(MAX_CAPACITY);

    std::string input;
    cin >> input;

    for (char ch : input)
    {
        tree.append(ch);
    }

    std::cout << tree.countSubstrings();

    return 0;
}
