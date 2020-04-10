/**
 * B. RSQ
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Входные данные
 * В первой строке находится число n — размер массива. (1 ≤ n ≤ 500 000)
 * Во второй строке находится n чисел ai — элементы массива.
 * Далее содержится описание операций, их количество не превышает 1 000 000.
 * В каждой строке находится одна из следующих операций:
 *
 * set i x — установить a[i] в x.
 * sum i j — вывести значение суммы элементов в массиве на отрезке с i по j, гарантируется, что (1 ≤ i ≤ j ≤ n).
 *
 * Все числа во входном файле и результаты выполнения всех операций не превышают по модулю 10^18.
 *
 * Выходные данные
 * Выведите последовательно результат выполнения всех операций sum. Следуйте формату выходного файла из примера.
 *
 * Пример:
 *
 * Входные данные
 * 5
 * 1 2 3 4 5
 * sum 2 5
 * sum 1 5
 * sum 1 4
 * sum 2 4
 * set 1 10
 * set 2 3
 * set 5 2
 * sum 2 5
 * sum 1 5
 * sum 1 4
 * sum 2 4
 *
 * Выходные данные
 * 14
 * 15
 * 10
 * 9
 * 12
 * 22
 * 20
 * 10
 */

#define DEBUG

#include <assert.h>
#include <cmath>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif

using value_t = long long;

void add(value_t* tree, const std::size_t size, const value_t value, std::size_t index)
{
    for (; index <= size; index = (index | (index + 1)))
    {
          tree[index] += value;
    }
}

value_t* buildFenvickTree(const value_t* data, const std::size_t size)
{
    value_t *tree = new value_t[size + 1];
    for (std::size_t i = 0; i <= size; ++i)
    {
        tree[i] = 0;
    }
    for (std::size_t i = 1; i <= size; ++i)
    {
        add(tree, size, data[i - 1], i);
    }
    return tree;
}

value_t get(const value_t* tree, const std::size_t size, std::size_t index)
{
    value_t result = 0;
    //++index;
    for (; index > 0 && index <= size; index -= index & -index)
    {
          result += tree[index];
    }
    return result;
}

value_t rsq(const value_t* tree, const std::size_t size, const std::size_t l, const std::size_t r)
{
    return l ? get(tree, size, r) - get(tree, size, l - 1) : get(tree, size, r);
}

void set(value_t* data, value_t* tree, const std::size_t size, const value_t value, std::size_t index)
{
    add(tree, size, value - data[index - 1], index);
    data[index - 1] = value;
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

    std::size_t dataSize;
    cin >> dataSize;
    value_t* data = new value_t[dataSize];
    for (std::size_t i = 0; i < dataSize; ++i)
    {
        cin >> data[i];
    }

    value_t* tree = buildFenvickTree(data, dataSize);

    std::string opType;
    while(cin >> opType)
    {
        if (opType[1] == 'e') // set
        {
            std::size_t index;
            value_t value;
            cin >> index >> value;
            set(data, tree, dataSize, value, index);
        }
        else if (opType[1] == 'u') // sum
        {
            std::size_t startIndex, endIndex;
            cin >> startIndex >> endIndex;
            std::cout << rsq(tree, dataSize, startIndex, endIndex) << '\n';
        }
    }

    delete[] tree;
    delete[] data;

    return 0;
}
