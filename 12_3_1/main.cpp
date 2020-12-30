/**
  * C. Множественный поиск 2
  *
  * ввод стандартный ввод
  * вывод стандартный вывод
  *
  * Дан массив строк s i и строка t.
  * Требуется для каждой строки s i определить, сколько раз она встречается в t как подстрока
  *
  * Входные данные
  * Первая строка входного файла содержит целое число n — число элементов в s (1 ≤ n ≤ 106).
  * Следующие n строк содержат по одной строке s i.
  * Сумма длин всех строк из s не превосходит 10^6.
  * Последняя строка входного файла содержит t (1 ≤ t ≤ 10^6).
  * Все строки состоят из строчных латинских букв.
  *
  * Выходные данные
  * For each line of s i print one number: how many times it occurs in t.
  * Lines are numbered in the order they appear in the input file.
  */

#define DEBUG

#include <assert.h>
#include <algorithm>
#include <iostream>
#include <limits>
#ifdef DEBUG
#include <fstream>
#endif
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
        std::size_t leafCount_;
        std::size_t children_[TAlphabetSize];

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
    std::size_t updateLeafCounts(std::size_t nodeIndex)
    {
        Node& node = nodes_[nodeIndex];
        unsigned long long result = node.right_ == sizeLimit_ ? 1 : 0;
        for (std::size_t childIndex : node.children_)
        {
            if (childIndex != std::numeric_limits<std::size_t>::max())
            {
                result += updateLeafCounts(childIndex);
            }
        }
        node.leafCount_ = result;
        return result;
    }
    bool findSubstring(const char* substring, std::size_t size, std::size_t& nodeIndex, std::size_t& remainder)
    {
        std::size_t index = 0;
        nodeIndex = 0;
        remainder = 0;
        if (nodes_[0].children_[substring[0] - TAlphabetOffset] == std::numeric_limits<std::size_t>::max())
        {
            return false;
        }
        while (index < size)
        {
            nodeIndex = nodes_[nodeIndex].children_[substring[index] - TAlphabetOffset];
            const Node& node = nodes_[nodeIndex];
            const std::size_t edgeLength = std::min(sequence_.size(), node.right_) - node.left_;
            if (index + edgeLength >= size)
            {
                for (remainder = 0; index < size; ++remainder, ++index)
                {
                    if (sequence_[node.left_ + remainder] != substring[index] - TAlphabetOffset)
                    {
                        return false;
                    }
                }
                return true;
            }
            else
            {
                if (node.children_[substring[index + edgeLength] - TAlphabetOffset] == std::numeric_limits<std::size_t>::max())
                {
                    return false;
                }
                for (std::size_t i = 0; i < edgeLength; ++i, ++index)
                {
                    if (sequence_[node.left_ + i] != substring[index] - TAlphabetOffset)
                    {
                        return false;
                    }
                }
            }
        }
        return false;
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
    void updateLeafCounts()
    {
        updateLeafCounts(0);
    }
    unsigned long long countSubstringEntries(const char* substring, std::size_t size)
    {
        std::size_t nodeIndex;
        std::size_t remainder;
        if (findSubstring(substring, size, nodeIndex, remainder))
        {
            return nodes_[nodeIndex].leafCount_;
        }
        return 0;
    }
};

static const std::size_t MAX_CAPACITY = 1000002;

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
    std::size_t substringCount;
    cin >> substringCount;
    std::vector<std::string> substrings(substringCount);
    for (std::size_t i = 0; i < substringCount; ++i)
    {
        cin >> substrings[i];
    }

    SuffixTree<27, 'a' - 1> tree(MAX_CAPACITY);
    std::string str;
    cin >> str;
    for (char ch : str)
    {
        tree.append(ch);
    }
    tree.append('a' - 1);
    tree.updateLeafCounts();
    for (std::size_t i = 0; i < substringCount; ++i)
    {
        std::cout << tree.countSubstringEntries(substrings[i].c_str(), substrings[i].length()) << '\n';
    }

    return 0;
}
