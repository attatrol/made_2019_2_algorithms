/**
 * A. Персистентный стек
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Реализуйте персистентный стек.
 *
 * Входные данные
 * Первая строка содержит количество действий n (1≤n≤200000).
 * В строке номер i+1 содержится описание действия i:
 * t m — добавить в конец стека номер t(0≤t<i) число m (0<m≤1000);
 * t 0 — удалить последний элемент стека номер t(0≤t<i).
 * Гарантируется, что стек t не пустой.
 * В результате действия i, описанного в строке i+1, создается стек номер i.
 * Изначально имеется пустой стек с номером ноль.
 * Все числа во входном файле целые.
 *
 * Выходные данные
 * Для каждой операции удаления выведите удаленный элемент на отдельной строке.
 */

#define DEBUG

#include <assert.h>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>
#include <unordered_set>

template<typename T>
class PersistentStack
{
private:
    struct Node
    {
        Node* next_;
        T value_;

        Node(): next_(nullptr)
        {
        }
        Node(Node* next, T value): next_(next), value_(value)
        {
        }
    };
    std::vector<Node*> heads_;
public:
    PersistentStack()
    {
        heads_.push_back(new Node());
    }
    ~PersistentStack()
    {
        std::unordered_set<Node*> nodes;
        for (std::size_t i = 0; i < heads_.size(); ++i)
        {
            nodes.insert(heads_[i]);
        }
        for (Node* node : nodes)
        {
            delete node;
        }
    }
    T pop(std::size_t version)
    {
        Node* node = heads_.at(version);
        heads_.push_back(node->next_);
        return node->value_;
    }
    void push(T value, std::size_t version)
    {
        Node* node = heads_.at(version);
        heads_.push_back(new Node(node, value));
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
    std::size_t opCount;
    cin >> opCount;

    PersistentStack<int> stack;
    for (std::size_t i = 0; i < opCount; ++i)
    {
        std::size_t version;
        int value;

        cin >> version >> value;

        if (value)
        {
            stack.push(value, version);
        }
        else
        {
            std::cout << stack.pop(version) << '\n';
        }
    }

    return 0;
}

