/**
  * K-я порядковая статистика на отрезке
  */

#define DEBUG

#include <assert.h>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <map>
#include <vector>

class PersistentSegmentTree
{
private:
    struct Node
    {
        std::size_t count_;
        Node* left_;
        Node* right_;

        Node(int count, Node* left,Node* right) : count_(count), left_(left), right_(right)
        {
        }
    };

    std::vector<int> indexesToValues_;
    std::map<int,int> valuesToIndexes_;
    std::size_t maxIndex_;
    Node* dummyNode_;
    std::vector<Node*> nodes_;

    Node *insert(Node* node, int nodeLeft, int nodeRight, int w)
    {
        if(nodeLeft <= w && w < nodeRight)
        {
            if(nodeLeft + 1 == nodeRight)
            {
                return new Node(node->count_ + 1, dummyNode_, dummyNode_);
            }
            const int mid=(nodeLeft + nodeRight) / 2;
            return new Node(node->count_ + 1, insert(node->left_, nodeLeft, mid, w), insert(node->right_, mid, nodeRight, w));
        }

        return node;
    }
    std::size_t query(Node *u, Node *v, std::size_t l, std::size_t r, std::size_t k)
    {
        if(l + 1 == r)
        {
            return l;
        }

        std::size_t mid = (l + r) / 2;
        int count = v->left_->count_ - u->left_->count_;
        if(count >= k)
        {
            return query(u->left_, v->left_, l, mid, k);
        }
        return query(u->right_, v->right_, mid, r, k - count);
    }
public:
    PersistentSegmentTree(std::vector<int> array) : maxIndex_(0), indexesToValues_(array.size()), dummyNode_(new Node(0, nullptr, nullptr)), nodes_(array.size())
    {
        for (std::size_t i = 0; i < array.size(); ++i)
        {
            valuesToIndexes_[array[i]];
        }

        for(auto it = valuesToIndexes_.begin(); it != valuesToIndexes_.end(); ++it)
        {
            valuesToIndexes_[it->first] = maxIndex_;
            indexesToValues_[maxIndex_] = it->first;
            ++maxIndex_;
        }

        dummyNode_->left_ = dummyNode_->right_ = dummyNode_;

        for(std::size_t i = 0; i < array.size(); ++i)
        {
            nodes_[i] = insert(i == 0 ? dummyNode_ : nodes_[i - 1], 0, maxIndex_, valuesToIndexes_[array[i]]);

        }
    }
    ~PersistentSegmentTree()
    {
        for (Node* node : nodes_)
        {
            delete node;
        }
    }
    int query(std::size_t l, std::size_t r, std::size_t k)
    {
        --l;
        --r;
        const std::size_t index = query((l == 0 ? dummyNode_ : nodes_[l - 1]), nodes_[r], 0, maxIndex_, k);
        return indexesToValues_[index];
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
    long long a, l, m;
    cin >> arraySize >> a >> l >> m;
    std::vector<int> array(arraySize);
    for (std::size_t i = 0; i < arraySize; ++i)
    {
        array[i] = a;
        a = (a * l + m) % 1000000000ll;
    }

#ifdef DEBUG
    for (std::size_t i = 0; i < arraySize; ++i)
    {
        std::cout << array[i] <<  ' ';
    }
    std::cout << '\n';
#endif

    PersistentSegmentTree tree(array);

    std::size_t queryCount;
    cin >> queryCount;

    long long querySum = 0;
    for (std::size_t i = 0; i < queryCount; ++i) {
        std::size_t queryCount1, x, lx, mx, y, ly, my, k, lk, mk;
        cin >> queryCount1 >> x >> lx >> mx >> y >> ly >> my >> k >> lk >> mk;
        std::size_t l = std::min(x, y);
        std::size_t r = std::max(x, y);
        for (std::size_t j = 0; j < queryCount1; ++j)
        {
            const int v = tree.query(l, r, k);
#ifdef DEBUG
            std::cout << "query l = " << l << " r = " << r << " kStat = " << k << " value = " << v << '\n';
#endif
            querySum += v;
            x = ((l - 1) * lx + mx) % arraySize + 1;
            y = ((r - 1) * ly + my) % arraySize + 1;
            l = std::min(x, y);
            r = std::max(x, y);
            k = ((k - 1) * lk + mk) % (r - l + 1) + 1;
        }
    }

    std::cout << querySum << '\n';
}
