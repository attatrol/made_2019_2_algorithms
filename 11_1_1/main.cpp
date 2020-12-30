/**
  * A. Суффиксный массив
  *
  * ввод стандартный ввод
  * вывод стандартный вывод
  *
  * Постройте суффиксный массив для заданной строки s, для каждых двух соседних суффиксов найдите длину максимального общего префикса.
  *
  * Входные данные
  * Первая строка входного файла содержит строку s (1 ≤ |s| ≤ 400 000).
  * Строка состоит из строчных латинских букв.
  *
  * Выходные данные
  * В первой строке выведите |s| различных чисел — номера первых символов суффиксов строки s так, чтобы соответствующие суффиксы были упорядочены в лексикографически возрастающем порядке.
  * Во второй строке выведите |s| - 1 чисел — длины наибольших общих префиксов.
  */

#define DEBUG

#include <assert.h>
#include <algorithm>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#include <iomanip>
#endif
#include <vector>

static const char ALPHABET_START_CHAR = 'a';
static const std::size_t ALPHABET_SIZE = 27;
static const char PLACEHOLDER_CHAR = ALPHABET_START_CHAR - 1;

std::vector<std::size_t> buildSuffixArray(const std::string& input, std::vector<std::size_t> &pos)
{
    const std::size_t length = input.length();
    std::vector<std::size_t> shifts(length);
    std::vector<std::size_t> classes(length);
    std::vector<std::size_t> cnt(ALPHABET_SIZE);

    // первая итерация - подсчет букв

    for (std::size_t i = 0; i < length; ++i)
    {
        ++cnt[input[i] - PLACEHOLDER_CHAR];
    }
    for (std::size_t i = 1; i < ALPHABET_SIZE; ++i)
    {
        cnt[i] += cnt[i - 1];
    }
    for (std::size_t i = 0; i < length; ++i)
    {
        shifts[--cnt[input[i] - PLACEHOLDER_CHAR]] = i;
    }

    std::size_t cls = 1;
    classes[shifts[0]] = 0;
    for (std::size_t i = 1; i < length; ++i)
    {
        if (input[shifts[i - 1]] != input[shifts[i]])
        {
            ++cls;
        }
        classes[shifts[i]] = cls - 1;
    }

    // вторая и последующие итерации

    std::vector<std::size_t> pn(length);
    std::vector<std::size_t> cn(length);
    for (std::size_t k = 0; (1 << k) < length; ++k)
    {
        for (std::size_t i = 0; i < length; ++i)
        {
            pn[i] = shifts[i] - (1 << k);
            if (pn[i] >= length)
            {
                pn[i] += length;
            }
        }

        // собственно bucket sort
        std::vector<std::size_t> counter(cls);
        for (std::size_t i = 0; i < pn.size(); ++i)
        {
            ++counter[classes[pn[i]]];
        }
        for (std::size_t i = 1; i < counter.size(); ++i)
        {
            counter[i] += counter[i - 1];
        }
        for (std::size_t i = length - 1; i < length; --i)
        {
            shifts[--counter[classes[pn[i]]]] = pn[i];
        }

        // перерасчет классов
        cls = 1;
        cn[shifts[0]] = 0;
        for (std::size_t i = 1; i < shifts.size(); ++i)
        {
            std::size_t temp1 = (shifts[i] + (1 << k)) % length;
            std::size_t temp2 = (shifts[i - 1] + (1 << k)) % length;
            if (classes[shifts[i]] != classes[shifts[i - 1]] || classes[temp1] != classes[temp2])
            {
                ++cls;
            }
            cn[shifts[i]] = cls - 1;
        }

        for (std::size_t i = 0; i < classes.size(); ++i)
        {
            classes[i] = cn[i];
        }
    }

    std::vector<std::size_t> result(shifts.size());
    for (std::size_t i = 0; i < shifts.size(); ++i)
    {
        result[i] = shifts[i];
        pos[result[i]] = i;
    }

    return result;
}


std::vector<std::size_t> buildLCP(const std::string& input, const std::vector<std::size_t>& suffixArray, const std::vector<std::size_t>& pos)
{
    std::vector<std::size_t> result(input.length());
    std::size_t current = 0;
    for (std::size_t i = 0; i < input.length() - 1; ++i)
    {
        if (current > 0)
        {
            --current;
        }
        if (pos[i] == input.length() - 1)
        {
            result[input.length() - 1] = 0;
            current = 0;
        }
        else
        {
            std::size_t j = suffixArray[pos[i] + 1];
            while (std::max(i + current, j + current) < input.length() && input[i + current] == input[j + current])
            {
                ++current;
            }
            result[pos[i]] = current;
        }
    }
    return result;
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

    std::string input;
    cin >> input;
    input += PLACEHOLDER_CHAR;
    std::vector<std::size_t> pos(input.length());
    std::vector<std::size_t> suffixArray = buildSuffixArray(input, pos);
    std::vector<std::size_t> lcp = buildLCP(input, suffixArray, pos);

    for(std::size_t i = 1; i < input.length(); ++i)
    {
        std::cout << suffixArray[i] + 1 << ' ';
    }
    std::cout << '\n';
    for (std::size_t i = 1; i < input.length() - 1; ++i)
    {
        std::cout << lcp[i] << " ";
    }

    return 0;
}
