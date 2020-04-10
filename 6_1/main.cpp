/**
 * A. Звезды
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Вася любит наблюдать за звездами.
 * Но следить за всем небом сразу ему тяжело.
 * Поэтому он наблюдает только за частью пространства, ограниченной кубом размером n × n × n.
 * Этот куб поделен на маленькие кубики размером 1 × 1 × 1.
 * Во время его наблюдений могут происходить следующие события:
 * 1. В каком-то кубике появляются или исчезают несколько звезд.
 * 2. К нему может заглянуть его друг Петя и поинтересоваться, сколько видно звезд в части пространства, состоящей из нескольких кубиков.
 *
 * Входные данные
 * Первая строка входного файла содержит натуральное число 1 ≤ n ≤ 128.
 * Координаты кубиков — целые числа от 0 до n - 1.
 * Далее следуют записи о происходивших событиях по одной в строке.
 * В начале строки записано число m.
 * Если m равно:
 * 1, то за ним следуют 4 числа — x, y, z (0 ≤ x, y, z < N) и k ( - 20000 ≤ k ≤ 20000) — координаты кубика и величина, на которую в нем изменилось количество видимых звезд;
 * 2, то за ним следуют 6 чисел — x1, y1, z1, x2, y2, z2 (0 ≤ x1 ≤ x2 < N, 0 ≤ y1 ≤ y2 < N, 0 ≤ z1 ≤ z2 < N), которые означают, что Петя попросил подсчитать количество звезд в кубиках (x, y, z) из области: x1 ≤ x ≤ x2, y1 ≤ y ≤ y2, z1 ≤ z ≤ z2;
 * 3, то это означает, что Васе надоело наблюдать за звездами и отвечать на вопросы Пети. Эта запись встречается во входном файле только один раз и будет последней.
 * Количество записей во входном файле не больше 100 002.
 *
 * Выходные данные
 * Для каждого Петиного вопроса выведите искомое количество звезд.
 */

#define DEBUG

#include <assert.h>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>

template<typename value_t>
class Fenwick3D{
private:
    std::size_t xSize_;
    std::size_t ySize_;
    std::size_t zSize_;
    std::vector<value_t> tree_;

    value_t query(std::size_t x, std::size_t y, std::size_t z);

public:
    Fenwick3D(std::size_t xSize, std::size_t ySize, std::size_t zSize);
    void add(std::size_t x, std::size_t y, std::size_t z, value_t value);
    value_t query(std::size_t x0, std::size_t y0, std::size_t z0, std::size_t x1, std::size_t y1, std::size_t z1);
};

template<typename value_t>
Fenwick3D<value_t>::Fenwick3D(std::size_t xSize, std::size_t ySize, std::size_t zSize) :
    xSize_(xSize + 1), ySize_(ySize + 1), zSize_(zSize + 1), tree_(xSize_ * ySize_ * zSize_, 0)
{
}

template<typename value_t>
value_t Fenwick3D<value_t>::query(std::size_t x, std::size_t y, std::size_t z)
{
    value_t result = 0;

    for(; x > 0; x -= x & -x)
    {
        for(std::size_t yTemp = y; yTemp > 0; yTemp -= yTemp & -yTemp)
        {
            for(std::size_t zTemp = z; zTemp > 0; zTemp -= zTemp & -zTemp)
            {
                assert(x + yTemp * xSize_ + zTemp * xSize_ * ySize_ < xSize_ * ySize_ * zSize_);
                result += tree_[x + yTemp * xSize_ + zTemp * xSize_ * ySize_];
            }
        }
    }
    return result;
}

template<typename value_t>
value_t Fenwick3D<value_t>::query(std::size_t x0, std::size_t y0, std::size_t z0, std::size_t x1, std::size_t y1, std::size_t z1)
{
    ++x0; ++x1; ++y0; ++y1; ++z0; ++z1;
    return query(x1,     y1,     z1    )
         - query(x0 - 1, y1,     z1    )
         - query(x1,     y0 - 1, z1    )
         - query(x1,     y1,     z0 - 1)
         + query(x0 - 1, y0 - 1, z1    )
         + query(x0 - 1, y1,     z0 - 1)
         + query(x1,     y0 - 1, z0 - 1)
         - query(x0 - 1, y0 - 1, z0 - 1);
}

template<typename value_t>
void Fenwick3D<value_t>::add(std::size_t x, std::size_t y, std::size_t z, value_t value)
{
    ++x; ++y; ++z;

    assert(x < xSize_|| y < ySize_|| z < zSize_);

    for(; x < xSize_; x += x & -x)
    {
        for(std::size_t yTemp = y; yTemp < ySize_; yTemp += yTemp & -yTemp)
        {
            for(std::size_t zTemp = z; zTemp < zSize_; zTemp += zTemp & -zTemp)
            {
                assert(x + yTemp * xSize_ + zTemp * xSize_ * ySize_ < xSize_ * ySize_ * zSize_);
                tree_[x + yTemp * xSize_ + zTemp * xSize_ * ySize_] += value;
            }
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
    cin.tie(0);
    std::cout.tie(0);
#endif

    std::size_t size;
    cin >> size;
    Fenwick3D<long long> tree(size, size, size);

    while (true)
    {
        std::size_t opCode;
        std::size_t x0, x1, y0, y1, z0, z1;
        long long value;
        cin >> opCode;
        switch (opCode) {
        case 1:
            cin >> x0 >> y0 >> z0 >> value;
            tree.add(x0, y0, z0, value);
            break;
        case 2:
            cin >> x0 >> y0 >> z0 >> x1 >> y1 >> z1;
            std::cout << tree.query(x0, y0, z0, x1, y1, z1) << '\n';
            break;
        case 3:
            goto _out;
        default: assert(false);

        }
    }
    _out:;

    return 0;
}
