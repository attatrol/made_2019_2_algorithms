/**
 * E. Циклические сдвиги
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * k-м циклическим сдвигом строки S называется строка, полученная перестановкой k первых символов строки S в конец строки.
 * Рассмотрим все различные циклические сдвиги строки S и отсортируем их по возрастанию.
 * Требуется вычислить i-ю строчку этого массива.
 * Например, для строки abacabac существует четыре различных циклических сдвига: нулевой (abacabac), первый (bacabaca), второй (acabacab) и третий (cabacaba).
 * После сортировки по возрастанию получится такой массив: abacabac, acabacab, bacabaca, cabacaba.
 *
 * Входные данные
 * В первой строке входного файла записана строка S, длиной не более 100 000 символов с ASCII-кодами от 32 до 126.
 * Во второй строке содержится единственное целое число k (1 ≤ k ≤ 100 000).
 *
 * Выходные данные
 * В выходной файл выведите k-й по возрастанию циклический сдвиг строки S, или слово IMPOSSIBLE, если такого сдвига не существует.
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

const int ALPHABET_SIZE = 100;
std::pair<std::vector<int>, std::vector<int>> buildSuffixArray(const std::string& str, std::vector<int> &pos)
{
    std::vector<std::size_t> p(str.length(), 0);
    std::vector<std::size_t> classes(str.length(), 0);
    std::vector<std::size_t> cnt(ALPHABET_SIZE, 0);

    for (std::size_t i = 0; i < str.length(); ++i)
    {
        ++cnt[str[i] - 32];
    }
    for (std::size_t i = 1; i < ALPHABET_SIZE; ++i)
    {
        cnt[i] += cnt[i - 1];
    }
    for (std::size_t i = 0; i < str.length(); ++i)
    {
        p[--cnt[str[i] - 32]] = i;
    }

    std::size_t cls = 1;
    classes[p[0]] = 0;
    for (int i = 1; i < p.size(); ++i)
    {
        if (str[p[i - 1]] != str[p[i]])
        {
            ++cls;
        }
        classes[p[i]] = cls - 1;
    }

    std::vector<int> pn(str.length());
    std::vector<int> cn(str.length());
    for (int k = 0; (1 << k) < str.length(); ++k)
    {
        for (int i = 0; i < p.size(); ++i)
        {
            pn[i] = p[i] - (1 << k);
            if (pn[i] < 0)
            {
                pn[i] += p.size();
            }
        }
        std::vector<int> counter(cls, 0);
        for (int i = 0; i < pn.size(); ++i)
        {
            ++counter[classes[pn[i]]];
        }
        for (int i = 1; i < counter.size(); ++i)
        {
            counter[i] += counter[i - 1];
        }
        for (int i = static_cast<int>( pn.size() - 1 ); i >= 0; --i)
        {
            p[--counter[classes[pn[i]]]] = pn[i];
        }

        cls = 1;
        cn[p[0]] = 0;
        for (int i = 1; i < p.size(); ++i)
        {
            int temp1 = (p[i] + (1 << k)) % p.size();
            int temp2 = (p[i - 1] + (1 << k)) % p.size();
            if (classes[p[i]] != classes[p[i - 1]] || classes[temp1] != classes[temp2])
            {
                ++cls;
            }
            cn[p[i]] = cls - 1;
        }

        for (int i = 0; i < classes.size(); ++i)
        {
            classes[i] = cn[i];
        }
    }

    std::vector<int> suffixArray(p.size());
    for (int i = 0; i < p.size(); ++i)
    {
        suffixArray[i] = p[i];
        pos[suffixArray[i]] = i;
    }

    return {suffixArray, cn};
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

    std::string str;
    cin >> str;
    int n = str.length();
    std::vector<int> pos(str.length());
    std::pair <std::vector<int>, std::vector<int>> result = buildSuffixArray(str, pos);
    str += str;
    int m;
    cin >> m;
    --m;
    for(int i = 0; i < n; ++i)
    {
        if(result.second[result.first[i]] == m)
        {
            for(int j = result.first[i]; j < result.first[i] + n; ++j)
            {
                std::cout << str[j];
            }
            return 0;
        }
    }
    std::cout << "IMPOSSIBLE";
    return 0;
}
