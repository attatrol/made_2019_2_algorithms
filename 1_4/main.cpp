/**
 * D. Расстояние по Левенштейну
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дана текстовая строка. С ней можно выполнять следующие операции:
 * 1. Заменить один символ строки на другой символ.
 * 2. Удалить один произвольный символ.
 * 3. Вставить произвольный символ в произвольное место строки.
 * Например, при помощи первой операции из строки «СОК» можно получить строку «СУК», при помощи второй операции — строку «ОК», при помощи третьей операции — строку «СТОК».
 * Минимальное количество таких операций, при помощи которых можно из одной строки получить другую, называется стоимостью редактирования или расстоянием Левенштейна.
 * Определите расстояние Левенштейна для двух данных строк.
 *
 * Входные данные
 * Программа получает на вход две строки, длина каждой из которых не превосходит 1000 символов, строки состоят только из заглавных латинских букв.
 *
 * Выходные данные
 * Требуется вывести одно число — расстояние Левенштейна для данных строк.
 */

//#define DEBUG

#include <algorithm>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif

std::size_t levensteinDistance(const std::string& str0, const std::string& str1)
{
    const std::size_t len0 = str0.length();
    const std::size_t len1 = str1.length();

    //   '; s; t; r; 1;
    // ';
    // s;
    // t;
    // r;
    // 0;
    std::size_t* matrix = new std::size_t[(len0 + 1) * (len1 + 1)];

    // initialize
    // distance between empty string and any string of length i is i
    for (std::size_t i = 0; i <= len1; ++i)
    {
        matrix[i] = i;
    }
    for (std::size_t i = 1, j = len1 + 1; i <= len0; ++i, j += len1 + 1)
    {
        matrix[j] = i;
    }

    // main cycle
    for (std::size_t i = 1; i <= len1; ++i)
    {
        for (std::size_t j = 1; j <= len0; ++j)
        {
            const std::size_t lt_lt = matrix[(j - 1) * (len1 + 1) + i - 1]; // replace
            if (str1[i - 1] == str0[j - 1])
            {
                // if terminal letters are the same then they don't add any distance
                matrix[j * (len1 + 1) + i] = lt_lt;
            }
            else
            {
                const std::size_t eq_lt = matrix[(j - 1) * (len1 + 1) + i];     // insert
                const std::size_t lt_eq = matrix[ j      * (len1 + 1) + i - 1]; // remove
                matrix[j * (len1 + 1) + i] = std::min({lt_lt, eq_lt, lt_eq}) + 1;
            }
        }
    }

    std::size_t result = matrix[(len0 + 1) * (len1 + 1) - 1];
    delete[] matrix;
    return result;
}

int main()
{
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
#endif

    std::string str0, str1;
    cin >> str0 >> str1;
    std::cout << levensteinDistance(str0, str1) << std::endl;
    return 0;
}
