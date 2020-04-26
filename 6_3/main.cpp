/**
 * C. Окна
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * На экране расположены прямоугольные окна, каким-то образом перекрывающиеся (со сторонами, параллельными осям координат).
 * Вам необходимо найти точку, которая покрыта наибольшим числом из них.
 *
 * Входные данные
 * В первой строке входного файла записано число окон n (1 ≤ n ≤ 50000).
 * Следующие n строк содержат координаты окон x_{1, i} y_{1, i} x_{2, i} y_{2, i},
 * где (x_{1, i}, y_{1, i}) — координаты левого верхнего угла i-го окна,
 * а (x_{2, i}, y_{2, i}) — правого нижнего (на экране компьютера y растет сверху вниз, а x — слева направо).
 * Все координаты — целые числа, по модулю не превосходящие 2·10^5.
 *
 * Выходные данные
 * В первой строке выходного файла выведите максимальное число окон, покрывающих какую-либо из точек в данной конфигурации.
 * Во второй строке выведите два целых числа, разделенные пробелом — координаты точки, покрытой максимальным числом окон.
 * Окна считаются замкнутыми, т.е. покрывающими свои граничные точки.
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

template<typename value_t>
class MaxSegmentTree
{
private:
    struct TreeNode
    {
        value_t value;
        value_t delayedSummand;
        bool set;
    };
    const std::size_t n_;
    const std::size_t rRoot_;
    const std::size_t size_;
    TreeNode* nodes_;

    inline constexpr std::size_t getPow2Ceil(std::size_t n)
    {
        std::size_t result = 1;
        while(result < n)
        {
            result <<= 1;
        }
        return result;
    }
    inline constexpr value_t getNeutralElement()
    {
        return std::numeric_limits<value_t>::min();
//        return std::numeric_limits<value_t>::max();
    }
    inline value_t operation(const value_t& a, const value_t& b)
    {
        return std::max(a, b);
//        return std::min(a, b);
    }
    value_t get(std::size_t index, std::size_t l, std::size_t r);
    void push(std::size_t index, std::size_t l, std::size_t r);
    void updateInner(std::size_t index, std::size_t l, std::size_t r, std::size_t a, std::size_t b, value_t value, bool set);
    value_t rmqInner(std::size_t index, std::size_t l, std::size_t r, std::size_t a, std::size_t b);

public:
    MaxSegmentTree(const value_t* data, std::size_t n): n_(n), rRoot_(getPow2Ceil(n)), size_(2 * rRoot_ - 1), nodes_(new TreeNode[size_])
    {

        for (std::size_t i = 0; i < n; ++i)
        {
            nodes_[rRoot_ + i - 1].value = data[i];
            nodes_[rRoot_ + i - 1].delayedSummand = 0;
            nodes_[rRoot_ + i - 1].set = false;
        }
        for (std::size_t i = n; i < rRoot_; ++i)
        {
            nodes_[rRoot_ + i - 1].value = getNeutralElement();
            nodes_[rRoot_ + i - 1].delayedSummand = 0;
            nodes_[rRoot_ + i - 1].set = false;
        }
        for (std::size_t i = rRoot_ - 2; i < rRoot_; --i)
        {
            nodes_[i].value = operation(nodes_[2 * i + 1].value, nodes_[2 * i + 2].value);
            nodes_[i].delayedSummand = 0;
            nodes_[i].set = false;
        }
    }

    MaxSegmentTree(std::size_t n): n_(n), rRoot_(getPow2Ceil(n)), size_(2 * rRoot_ - 1), nodes_(new TreeNode[size_])
    {
        for (std::size_t i = 0; i < size_; ++i)
        {
            nodes_[i].value = 0; //getNeutralElement();
            nodes_[i].delayedSummand = 0;
            nodes_[i].set = false;
        }
    }

    ~MaxSegmentTree()
    {
        delete[] nodes_;
    }

    void update(std::size_t a, std::size_t b, value_t value, bool set)
    {
        updateInner(0, 1, rRoot_, a, b, value, set);
    }

    value_t rmq(std::size_t a, std::size_t b)
    {
        return rmqInner(0, 1, rRoot_, a, b);
    }
};

template<typename value_t>
value_t MaxSegmentTree<value_t>::get(std::size_t index, std::size_t l, std::size_t r)
{
    assert(l <= r);
    if (nodes_[index].set)
    {
        return nodes_[index].delayedSummand;
    }
    else
    {
        return nodes_[index].delayedSummand + nodes_[index].value;
    }
}

template<typename value_t>
void MaxSegmentTree<value_t>::push(std::size_t index, std::size_t l, std::size_t r)
{
    assert(l <= r);
    if (l == r)
    {
        if (nodes_[index].set)
        {
            nodes_[index].value = nodes_[index].delayedSummand;
            nodes_[index].set = false;
        }
        else
        {
            nodes_[index].value += nodes_[index].delayedSummand;
        }
        nodes_[index].delayedSummand = 0;
    }
    else
    {
        if (nodes_[index].set)
        {
            nodes_[2 * index + 1].delayedSummand = nodes_[2 * index + 2].delayedSummand = nodes_[index].delayedSummand;
            nodes_[2 * index + 1].set = nodes_[2 * index + 2].set = true;
        }
        else
        {
            nodes_[2 * index + 1].delayedSummand += nodes_[index].delayedSummand;
            nodes_[2 * index + 2].delayedSummand += nodes_[index].delayedSummand;
        }
        std::size_t m = (l + r) / 2;
        nodes_[index].delayedSummand = 0;
        nodes_[index].set = false;
        nodes_[index].value = operation(get(2 * index + 1, l, m), get(2 * index + 2, m + 1, r));
    }
}

template<typename value_t>
void MaxSegmentTree<value_t>::updateInner(std::size_t index, std::size_t l, std::size_t r, std::size_t a, std::size_t b, value_t value, bool set)
{
    assert(l <= r);
    assert(a <= b);
    if (nodes_[index].delayedSummand || nodes_[index].set)
    {
        push(index, l, r);
    }
    if (l > b || r < a)
    {
        return;
    }
    if (l >= a && r <= b)
    {
        nodes_[index].delayedSummand = value;
        nodes_[index].set = set;
        return;
    }
    std::size_t m = (l + r) / 2;
    updateInner(2 * index + 1, l, m, a, b, value, set);
    updateInner(2 * index + 2, m + 1, r, a, b, value, set);
    nodes_[index].value = operation(get(2 * index + 1, l, m), get(2 * index + 2, m + 1, r));
}

template<typename value_t>
value_t MaxSegmentTree<value_t>::rmqInner(std::size_t index, std::size_t l, std::size_t r, std::size_t a, std::size_t b)
{
    assert(l <= r);
    assert(a <= b);
    assert(index < size_);
    if (nodes_[index].delayedSummand || nodes_[index].set)
    {
        push(index, l, r);
    }
    if (l > b || r < a) return getNeutralElement();
    if (l >= a && r <= b) return get(index, l, r);
    std::size_t m = (l + r) / 2;
    return operation(rmqInner(2 * index + 1, l, m, a, b), rmqInner(2 * index + 2, m + 1, r, a, b));
}

struct Range
{
    int x0, x1, y;
    bool closing;
};

bool rangeSorter(const Range& lhs, const Range& rhs) {
    if (lhs.y == rhs.y)
    {
        return lhs.closing && !rhs.closing;
    }
    return lhs.y < rhs.y;
}

static const int COORD_OFFSET = 200001;
static const std::size_t SEGMENT_TREE_SIZE = 400002;

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

    std::size_t rectCount;
    cin >> rectCount;

    Range* ranges = new Range[2 * rectCount];

    for (std::size_t i = 0; i < rectCount; ++i)
    {
        int x0, x1, y0, y1;
        cin >> x0 >> y0 >> x1 >> y1;
        x0 += COORD_OFFSET;
        x1 += COORD_OFFSET;
        y0 += COORD_OFFSET;
        y1 += COORD_OFFSET;
        assert(1 <= x0 && x0 <= x1 && x1 <= SEGMENT_TREE_SIZE);
        assert(1 <= y0 && y0 <= y1 && y1 <= SEGMENT_TREE_SIZE);
        ranges[2 * i].x0 = x0;
        ranges[2 * i].x1 = x1;
        ranges[2 * i].y = y0;
        ranges[2 * i].closing = false;
        ranges[2 * i + 1].x0 = x0;
        ranges[2 * i + 1].x1 = x1;
        ranges[2 * i + 1].y = y1 + 1;
        ranges[2 * i + 1].closing = true;
    }

    std::sort(ranges, ranges + 2 * rectCount, &rangeSorter);

    MaxSegmentTree<int> tree(SEGMENT_TREE_SIZE);

    int maxY = -1;
    int maxCountY = -1;

    for (std::size_t i = 0; i < 2 * rectCount; ++i)
    {
#ifdef DEBUG
        std::cout << "x0=" << (ranges[i].x0 - COORD_OFFSET)
                  << " x1=" << (ranges[i].x1 - COORD_OFFSET)
                  << " y=" << (ranges[i].y - COORD_OFFSET)
                  << " closing=" << ranges[i].closing;
#endif
        if (ranges[i].closing)
        {
            tree.update(ranges[i].x0, ranges[i].x1, -1, false);
        }
        else
        {
            tree.update(ranges[i].x0, ranges[i].x1, 1, false);
            const int countY = tree.rmq(1, SEGMENT_TREE_SIZE);
            assert(countY >= 0);
#ifdef DEBUG
            std::cout << " countY=" << countY;
#endif
            if (countY > maxCountY)
            {
                maxY = ranges[i].y;
                maxCountY = countY;
            }
        }
#ifdef DEBUG
        std::cout << '\n';
#endif
    }

    assert(tree.rmq(1, SEGMENT_TREE_SIZE) == 0);

    bool found = false;
    for (std::size_t i = 0; i < 2 * rectCount; ++i)
    {
        if (ranges[i].closing)
        {
            tree.update(ranges[i].x0, ranges[i].x1, -1, false);
        }
        else
        {
            tree.update(ranges[i].x0, ranges[i].x1, 1, false);
            if (ranges[i].y == maxY && tree.rmq(1, SEGMENT_TREE_SIZE) == maxCountY)
            {
                found  = true;
                // binary search for X
                std::size_t a = 1;
                std::size_t b = SEGMENT_TREE_SIZE;
                while (a != b)
                {
                    std::size_t m = (a + b) / 2;
                    if (tree.rmq(a, m) == maxCountY)
                    {
                        b = m;
                    }
                    else
                    {
                        a = m + 1;
                    }
                    assert(a <= b);
                }
                std::cout << maxCountY << '\n' << (static_cast<int>(a) - COORD_OFFSET) << ' ' << (maxY - COORD_OFFSET);
                break;
            }
        }
    }
    assert(found);

    delete[] ranges;
    return 0;
}

