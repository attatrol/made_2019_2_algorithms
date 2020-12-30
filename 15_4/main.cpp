/**
  * D. Менеджер памяти
  *
  * ввод стандартный ввод
  * вывод стандартный вывод
  *
  * Одно из главных нововведений новейшей операционной системы Indows 7 — новый менеджер памяти.
  * Он работает с массивом длины N и позволяет выполнять три самые современные операции:
  * copy(a, b, l) — скопировать отрезок длины [a,a+l−1] в [b,b+l−1]
  * sum(l, r) — посчитать сумму элементов массива на отрезке [l,r]
  * print(l, r) — напечатать элементы с по r, включительно
  * Вы являетесь разработчиком своей операционной системы, и Вы, безусловно, не можете обойтись без инновационных технологий.
  * Вам необходимо реализовать точно такой же менеджер памяти.
  *
  * Входные данные
  * Первая строка входного файла содержит целое число N(1≤N≤1000000) — размер массива, с которым будет работать Ваш менеджер памяти.
  * Во второй строке содержатся четыре числа 1≤X_1,A,B,M≤10^9+10.
  * С помощью них можно сгенерировать исходный массив чисел X_1,X_2,...,X_N. X_{i+1}=(A∗X_i+B) mod M
  * Следующая строка входного файла содержит целое число K (1≤K≤200000) — количество запросов, которые необходимо выполнить Вашему менеджеру памяти.
  * Далее в K строках содержится описание запросов. Запросы заданы в формате:
  * cpy a b l — для операции copy
  * sum l r — для операции sum (l≤r)
  * out l r — для операции print (l≤r)
  * Гарантируется, что суммарная длина запросов print не превышает 3000.
  * Также гарантируется, что все запросы корректны.
  *
  * Выходные данные
  * Для каждого запроса sum или print выведите в выходной файл на отдельной строке результат запроса.
  */

#define DEBUG

#include <assert.h>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>

struct Node {
    long long int value_;
    int count_;
    long long int sum_;
    int link_;
    Node *left_;
    Node *right_;
    Node()
    {
        link_ = 0;
        value_ = 0;
        count_ = 0;
        sum_ = 0;
        left_ = nullptr;
        right_ = nullptr;
    }
    Node(long long int value)
    {
        value_ = value;
        count_ = 1;
        sum_ = value;
        link_ = 0;
        left_ = nullptr;
        right_ = nullptr;
    }
    Node(const Node* node) {
        if (node == nullptr)
        {
            return;
        }
        value_ = node->value_;
        count_ = node->count_;
        sum_ = node->sum_;
        link_ = 0;
        left_ = node->left_;
        right_ = node->right_;
    }
};

void incLink(Node* node)
{
    if (node == nullptr)
    {
        return;
    }
    node->link_++;
}
void deleteNode(Node* node)
{
    if (node == nullptr)
    {
        return;
    }
    node->link_--;
    if (node->link_ <= 0) {
        deleteNode(node->left_);
        deleteNode(node->right_);
        delete node;
    }
}
int getCount(Node* node) {
    if (node == nullptr)
    {
        return 0;
    }
    return node->count_;
}
long long int getSum(Node* node)
{
    if (node == nullptr)
    {
        return 0;
    }
    return node->sum_;
}
void updateNode(Node* node)
{
    if (node != nullptr)
    {
        node->count_ = getCount(node->right_) + getCount(node->left_) + 1;
        node->sum_ = getSum(node->right_) + getSum(node->left_) + node->value_;
    }
}
Node* mergeNodes(Node* leftNode, Node* rightNode) {
    Node* node = nullptr;
    if (leftNode == nullptr)
    {
        node = new Node(rightNode);
        incLink(node->left_);
        incLink(node->right_);
        return node;
    }
    if (rightNode == nullptr) {
        node = new Node(leftNode);
        incLink(node->left_);
        incLink(node->right_);
        return node;
    }
    if ((rand() % (leftNode->count_ + rightNode->count_)) > rightNode -> count_)
    {
        node = new Node(leftNode);
        node->right_ = mergeNodes(node->right_, rightNode);
        incLink(node->left_);
        incLink(node -> right_);
        updateNode(node);
        return node;
    }
    else
    {
        node = new Node(rightNode);
        node->left_ = mergeNodes(leftNode, node->left_);
        incLink(node->left_);
        incLink(node->right_);
        updateNode(node);
        return node;
    }
}
void splitNode(Node* node, int x, Node*& leftNode, Node*& rightNode)
{
    if (node == nullptr)
    {
        leftNode = nullptr;
        rightNode = nullptr;
        return;
    }
    Node* currentNode = new Node(node);
    int indexx = getCount(node->left_) + 1;
    if (indexx <= x)
    {
        splitNode(currentNode->right_, x - indexx, currentNode->right_, rightNode);
        incLink(currentNode->left_);
        incLink(currentNode->right_);
        leftNode = currentNode;
    }
    else
    {
        splitNode(currentNode->left_, x, leftNode, currentNode->left_);
        incLink(currentNode->left_);
        incLink(currentNode->right_);
        rightNode = currentNode;
    }
    updateNode(leftNode);
    updateNode(rightNode);
}
void printNode(Node* node)
{
    if (node == nullptr)
    {
        return;
    }
    printNode(node -> left_);
    std::cout << node->value_ << ' ';
    printNode(node -> right_);
}
int main() {
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    std::cout.tie(nullptr);
#endif

    std::size_t N;
    long long int X1, A, B, M;
    cin >> N >> X1 >> A >> B >> M;

    Node *t = nullptr;
    for (std::size_t i = 0; i < N; i++)
    {
        Node *tt;
        Node *nt;
        if (i == 0)
        {
            tt = new Node(X1);
            nt = mergeNodes(t, tt);
            deleteNode(t);
            deleteNode(tt);
            t = nt;
        }
        else
        {
            X1 = (A * X1 + B) % M;
            tt = new Node(X1);
            nt = mergeNodes(t, tt);
            deleteNode(t);
            deleteNode(tt);
            t = nt;
        }
    }

    int K;
    cin >> K;

    std::string s;
    int a, b, w;
    for (int i = 0; i < K; i++)
    {
        cin >> s;
        if (s == "cpy")
        {
            cin >> a >> b >> w;
            a--; b--;
            Node *left = nullptr;
            Node *right = nullptr;
            Node *lleft = nullptr;
            Node *rright = nullptr;
            Node *lr = nullptr;
            Node *sr = nullptr;
            Node *nt;
            splitNode(t, a, left, right);
            deleteNode(left);
            sr = right;
            splitNode(right, w, left, right);
            deleteNode(sr);
            deleteNode(right);
            sr = nullptr;

            splitNode(t, b, lleft, rright);
            lr = lleft;
            sr = rright;
            splitNode(rright, w, lleft, rright);
            deleteNode(lleft);
            deleteNode(sr);
            Node *tnt;
            nt = mergeNodes(lr, left);
            tnt = nt;
            nt = mergeNodes(nt, rright);
            deleteNode(lr);
            deleteNode(left);
            deleteNode(rright);
            deleteNode(tnt);
            deleteNode(t);
            t = nt;
        }
        else if (s == "sum")
        {
            cin >> a >> b;
            a--;
            b -= a;
            Node *left;
            Node *right;
            Node *sr;
            splitNode(t, a, left, right);
            deleteNode(left);
            sr = right;
            splitNode(right, b, left, right);
            deleteNode(sr);
            deleteNode(right);
            std::cout << left->sum_ << '\n';
            deleteNode(left);
        }
        else if (s == "out")
        {
            cin >> a >> b;
            a--;
            b -= a;
            Node *left;
            Node *right;
            Node *sr;
            splitNode(t, a, left, right);
            deleteNode(left);
            sr = right;
            splitNode(right, b, left, right);
            deleteNode(sr);
            deleteNode(right);
            printNode(left);
            std::cout << '\n';
            deleteNode(left);
        }
    }
    return 0;
}
