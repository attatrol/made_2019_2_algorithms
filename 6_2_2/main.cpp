/**
 * B. Ферма
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Настала весна и фермер решил заняться удобрением своего земельного участка размерами x×y метров.
 * Для этого он закупил удобрения.
 * До начала посевов остаётся n дней, и фермер хочет успеть сделать как можно больше.
 * За день фермер может одну из следующих вещей:
 *   увеличить продуктивность прямоугольного участка земли со сторонами, параллельными осям координат с углами (x1,y1) и (x2,y2) на значение w
 *   посчитать суммарную продуктивность участка (x1,y1) — (x2,y2)
 * Удобрять фермер любит сам, а вот заниматься скучными расчетами ему не интересно.
 * Помогите ему в этом.
 *
 * Входные данные
 * В первой строке входного файла записаны числа x и y (1≤x,y≤1000).
 * В следующей строке написано количество оставшихся до начала посевов дней n (1≤n≤100000).
 * Следующие n строк описывают действия фермера в соответственный день в следующем формате:
 * 1 x1 y1 x2 y2 w — фермер удобряет участок. (1≤x1≤x2≤x, 1≤y1≤y2≤y,−10000≤w≤10000)
 * 2 x1 y1 x2 y2 — фермер просит посчитать плодородность участка. (1≤x1≤x2≤x, 1≤y1≤y2≤y)
 *
 * Выходные данные
 * Для каждого запроса плодородности участка в отдельной строке выведите плодородность этого участка.
 */

//#define DEBUG

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#include <iomanip>
#endif
#include <limits>

constexpr std::size_t getPow2Ceil(std::size_t n)
{
    std::size_t result = 1;
    while(result < n)
    {
        result <<= 1;
    }
    return result;
}


template<typename value_t>
class SegmentTree
{
private:
    struct Node
    {
        value_t x;
        value_t y;
        value_t xDeferred;
        value_t yDeferred;
    };

    const std::size_t x_;
    const std::size_t y_;
    const std::size_t xSize_;
    const std::size_t ySize_;
    Node* tree_;

    inline std::size_t getCoord(std::size_t x, std::size_t y)
    {
        assert(x < xSize_);
        assert(y < ySize_);
        return x + xSize_ * y;
    }
    void updateByX(std::size_t coordX, std::size_t xL, std::size_t xR,
                   std::size_t x0, std::size_t x1, std::size_t y0, std::size_t y1, value_t value);

    void updateByY(std::size_t coordX, std::size_t coordY,
                   std::size_t xL, std::size_t xR, std::size_t yL, std::size_t yR,
                   std::size_t x0, std::size_t x1, std::size_t y0, std::size_t y1, value_t value, bool subset);

    value_t queryByX(std::size_t coordX, std::size_t coordY, std::size_t xL, std::size_t xR,
                     std::size_t x0, std::size_t x1, std::size_t y0, std::size_t y1);

    value_t queryByY(std::size_t coordX, std::size_t coordY,
                     std::size_t xL, std::size_t xR, std::size_t yL, std::size_t yR,
                     std::size_t x0, std::size_t x1, std::size_t y0, std::size_t y1, value_t deferred);
public:
    SegmentTree(std::size_t x, std::size_t y);
    SegmentTree(const SegmentTree&) = delete;
    SegmentTree& operator=(const SegmentTree&) = delete;
    SegmentTree(SegmentTree&&) = delete;
    SegmentTree& operator=(SegmentTree&&) = delete;
    ~SegmentTree()
    {
        delete[] tree_;
    }
    void update(std::size_t x0, std::size_t x1, std::size_t y0, std::size_t y1, value_t value)
    {
        updateByX(1, 1, x_, x0, x1, y0, y1, value);
    }
    value_t query(std::size_t x0, std::size_t x1, std::size_t y0, std::size_t y1)
    {
        return queryByX(1, 1, 1, x_, x0, x1, y0, y1);
    }

};

template<typename value_t>
SegmentTree<value_t>::SegmentTree(std::size_t x, std::size_t y) :
    x_(x), y_(y), xSize_(2 * getPow2Ceil(x) + 1), ySize_(2 * getPow2Ceil(y) + 1), tree_(new Node[xSize_ * ySize_])
{
    for (std::size_t i = 0; i < xSize_ * ySize_; ++i)
    {
        tree_[i].x = tree_[i].y = tree_[i].xDeferred = tree_[i].yDeferred = 0;
    }
}

template<typename value_t>
void SegmentTree<value_t>::updateByX(std::size_t coordX, std::size_t xL, std::size_t xR,
                                     std::size_t x0, std::size_t x1, std::size_t y0, std::size_t y1, value_t value)
{
    if (x0 <= xL && xR <= x1)
    {
        updateByY(coordX, 1, xL, xR, 1, y_, x0, x1, y0, y1, value, true);
        return;
    }
    else if (xL > x1 || xR < x0)
    {
        return;
    }
    else
    {
        const std::size_t xM = (xL + xR) / 2;
        updateByX(coordX * 2,     xL,      xM, x0, x1, y0, y1, value);
        updateByX(coordX * 2 + 1, xM + 1, xR,  x0, x1, y0, y1, value);
        const std::size_t trueXL = std::max(x0, xL);
        const std::size_t trueXR = std::min(x1, xR);
        updateByY(coordX, 1, xL, xR, 1, y_, trueXL, trueXR, y0, y1, value * (trueXR - trueXL + 1) / (xR - xL + 1), false);
    }
}

template<typename value_t>
void SegmentTree<value_t>::updateByY(std::size_t coordX, std::size_t coordY,
                                     std::size_t xL, std::size_t xR, std::size_t yL, std::size_t yR,
                                     std::size_t x0, std::size_t x1, std::size_t y0, std::size_t y1, value_t value, bool subset)
{
    const std::size_t coord = getCoord(coordX, coordY);

    if (y0 <= yL && yR <= y1)
    {
        if (subset)
        {
            tree_[coord].xDeferred += value;
            tree_[coord].x += value * (xR - xL + 1) * (yR - yL + 1);
        }
        else
        {
            tree_[coord].yDeferred += value;
            tree_[coord].y += value * (xR - xL + 1) * (yR - yL + 1);
        }
        return;
    }
    else if (yR < y0 || y1 < yL)
    {
        return;
    }
    else
    {
        const std::size_t yM = (yL + yR) / 2;
        updateByY(coordX, coordY * 2,     xL, xR, yL,      yM, x0, x1, y0, y1, value, subset);
        updateByY(coordX, coordY * 2 + 1, xL, xR, yM + 1, yR,  x0, x1, y0, y1, value, subset);
        const std::size_t leftCoord  = getCoord(coordX, coordY * 2);
        const std::size_t rightCoord = getCoord(coordX, coordY * 2 + 1);
        tree_[coord].y = tree_[leftCoord].y + tree_[rightCoord].y + tree_[coord].yDeferred * (xR - xL + 1) * (yR - yL + 1);
        tree_[coord].x = tree_[leftCoord].x + tree_[rightCoord].x + tree_[coord].xDeferred * (xR - xL + 1) * (yR - yL + 1);
    }
}

template<typename value_t>
value_t SegmentTree<value_t>::queryByX(std::size_t coordX, std::size_t coordY,
                                       std::size_t xL, std::size_t xR, std::size_t x0, std::size_t x1, std::size_t y0, std::size_t y1)
{
    if (x0 <= xL && xR <= x1)
    {
        return queryByY(coordX, 1, xL, xR, 1, y_, x0, x1, y0, y1, 0);
    }
    else if (xL > x1 || xR < x0)
    {
        return 0;
    }
    else
    {
        const std::size_t xM = (xL + xR) / 2;
        const std::size_t trueXL = std::max(x0, xL);
        const std::size_t trueXR = std::min(x1, xR);
        return queryByY(coordX, coordY, xL, xR, 1, y_, trueXL, trueXR, y0, y1, 0)
                + queryByX(coordX * 2,     coordY, xL,      xM, x0, x1, y0, y1)
                + queryByX(coordX * 2 + 1, coordY, xM + 1, xR,  x0, x1, y0, y1);
    }
}

template<typename value_t>
value_t SegmentTree<value_t>::queryByY(std::size_t coordX, std::size_t coordY,
                                       std::size_t xL, std::size_t xR, std::size_t yL, std::size_t yR,
                                       std::size_t x0, std::size_t x1, std::size_t y0, std::size_t y1, value_t deferred)
{
    const std::size_t coord = getCoord(coordX, coordY);

    if (y0 <= yL && yR <= y1)
    {
        if (x0 <= xL && xR <= x1)
        {
            return tree_[coord].y + tree_[coord].x + deferred * (xR - xL + 1) * (yR - yL + 1);
        }
        else if (xL > x1 || xR < x0)
        {
            return 0;
        }
        else
        {
            return tree_[coord].x * (x1 - x0 + 1) / (xR - xL + 1) + deferred * (x1 - x0 + 1) * (yR - yL + 1);
        }
    }
    else if (yL > y1 || yR < y0)
    {
        return 0;
    }
    else
    {
        const std::size_t yM = (yL + yR) / 2;
        if (x0 <= xL && xR <= x1)
        {
            return queryByY(coordX, coordY * 2,     xL, xR, yL,      yM, x0, x1, y0, y1, deferred + tree_[coord].xDeferred + tree_[coord].yDeferred)
                    + queryByY(coordX, coordY * 2 + 1, xL, xR, yM + 1, yR,  x0, x1, y0, y1, deferred + tree_[coord].xDeferred + tree_[coord].yDeferred);
        }
        else if (xL > x1 || xR < x0)
        {
            return 0;
        }
        else
        {
            return queryByY(coordX, coordY * 2,     xL, xR, yL,      yM, x0, x1, y0, y1, deferred + tree_[coord].xDeferred)
                    + queryByY(coordX, coordY * 2 + 1, xL, xR, yM + 1, yR,  x0, x1, y0, y1, deferred + tree_[coord].xDeferred);
        }
    }
}

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

    std::size_t x, y, opCount;
    cin >> x >> y >> opCount;
    SegmentTree<double> tree(x, y);
//    tree.printOut();

    for (std::size_t i = 0; i < opCount; ++i)
    {
        std::size_t op, x0, x1, y0, y1;
        cin >> op >> x0 >> y0 >> x1 >> y1;
        switch (op)
        {
        case 1:
            long long value;
            cin >> value;
            tree.update(x0, x1, y0, y1, value);
//            tree.printOut();
            break;
        case 2:
            std::cout << std::llround(tree.query(x0, x1, y0, y1)) << '\n';
//            tree.printOut();
            break;
        default:
            assert(false);
        }
    }

    return 0;
}
