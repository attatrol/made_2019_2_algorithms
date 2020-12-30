/**
 * B. Персистентный массив
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дан массив (вернее, первая, начальная его версия).
 * Нужно уметь отвечать на два запроса:
 * ∘ a_i[j]=x — создать из i-й версии новую, в которой j-й элемент равен x, а остальные элементы такие же, как в i-й версии.
 * ∘ get a_i[j] — сказать, чему равен j-й элемент в i-й версии.
 *
 * Входные данные
 * Количество чисел в массиве N(1≤N≤10^5) и N элементов массива.
 * Далее количество запросов M (1≤M≤10^5) и M запросов.
 * Формат описания запросов можно посмотреть в примере.
 * Если уже существует K версий, новая версия получает номер K+1.
 * И исходные, и новые элементы массива — целые числа от 0 до 10^9.
 * Элементы в массиве нумеруются числами от 1 до N
 *
 * Выходные данные
 * На каждый запрос типа get вывести соответствующий элемент нужного массива.
 */

#define DEBUG

#include <assert.h>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>
#include <unordered_set>

constexpr std::size_t getMaxPow2(std::size_t n)
{
    std::size_t res = 1;
    while (res < n)
    {
        res <<= 1;
    }
    return res;
}

template<typename T>
class PersistentBinaryTree
{
private:
    struct Node
    {
        const Node* left_;
        const Node* right_;
        T value_;

        Node(T value): left_(nullptr), right_(nullptr), value_(value)
        {
        }
        Node(const Node* left, const Node* right): left_(left), right_(right)
        {
        }
    };
    std::vector<const Node*> heads_;
    std::vector<const Node*> nodes_;
    const std::size_t mask_;
public:
    PersistentBinaryTree(const std::vector<T>& data) : mask_(getMaxPow2(data.size()) >> 1)
    {
        std::vector<Node*> nodes(data.size());
        for (std::size_t i = 0; i < data.size(); ++i)
        {
            nodes[i] = new Node(data[i]);
            nodes_.push_back(nodes[i]);
        }
        while (nodes.size() > 1)
        {
            std::vector<Node*> nextLayerNodes(nodes.size() / 2);
            std::size_t i = 0;
            for (; i < nodes.size() / 2; ++i)
            {
                assert(2 * i + 1 < nodes.size());
                nextLayerNodes[i] = new Node(nodes[2 * i], nodes[2 * i + 1]);
                nodes_.push_back(nextLayerNodes[i]);
            }
            assert(2 * i == nodes.size() || 2 * i == nodes.size() - 1);
            if (2 * i < nodes.size())
            {
                nextLayerNodes.push_back(new Node(nodes[2 * i], nullptr));
                nodes_.push_back(nextLayerNodes.back());
            }
            nodes = nextLayerNodes;
        }
        assert(nodes.size() == 1);
        heads_.push_back(nodes[0]);
    }
    ~PersistentBinaryTree()
    {
        for (const Node* node : nodes_)
        {
            delete node;
        }
    }
    T get(std::size_t index, std::size_t version)
    {

        const Node* node = heads_.at(version);
        std::size_t mask = mask_;
        while (mask)
        {
            node = (mask & index) ? node->right_ : node->left_;
            assert(node);
            mask >>= 1;
        }
        assert(!node->left_ && !node->right_);
        return node->value_;
    }
    void insert(T value, std::size_t index, std::size_t version)
    {
        const Node* node = heads_.at(version);
        std::vector<std::pair<const Node*, bool>> path;
        std::size_t mask = mask_;
        while (mask)
        {
            bool rightTurn = mask & index;
            path.push_back(std::make_pair(node, rightTurn));
            node = rightTurn ? node->right_ : node->left_;
            assert(node);
            mask >>= 1;
        }
        node = new Node(value);
        nodes_.push_back(node);
        for (auto it = path.rbegin(); it < path.rend(); ++it)
        {
            if (it->second)
            {
                node = new Node(it->first->left_, node);
            }
            else
            {
                node = new Node(node, it->first->right_);
            }
            nodes_.push_back(node);
        }
        heads_.push_back(node);
    }
};

int main() {
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    std::cout.tie(nullptr);
#endif
    std::size_t arraySize;
    cin >> arraySize;

    std::vector<int> array(arraySize);
    for (std::size_t i = 0; i < arraySize; ++i)
    {
        cin >> array[i];
    }
    PersistentBinaryTree<int> tree(array);

    std::size_t opCount;
    cin >> opCount;
    for (std::size_t i = 0; i < opCount; ++i)
    {
        std::string op;
        std::size_t version, index;
        cin >> op >> version >> index;
        --version;
        --index;

        if (op[0] == 'g')
        {
            std::cout << tree.get(index, version) << '\n';
        }
        else
        {
            int value;
            cin >> value;
            tree.insert(value, index, version);
        }
    }

    return 0;
}

