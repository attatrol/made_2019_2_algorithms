/**
 * A. Массовое разложение на множители
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дано много чисел.
 * Требуется разложить их все на простые множители.
 *
 * Входные данные
 * В первой строке задано число n (2 ≤ n ≤ 300000).
 * В следующих n строках заданы числа ai (2 ≤ ai ≤ 106), которые нужно разложить на множители.
 *
 * Выходные данные
 * Для каждого числа выведите в отдельной строке разложение на простые множители в порядке возрастания множителей.
 */

#define DEBUG

#include <assert.h>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>

static const int MAX_VALUE = 1000000;

void buildSieve(int* addrCache, int* factorCache)
{
    std::fill(factorCache, factorCache + MAX_VALUE + 1, 0);
    for (int i = 2; i <= MAX_VALUE; ++i)
    {
        if (!factorCache[i])
        {
            for (int j = i; j <= MAX_VALUE; j += i)
            {
                factorCache[j] = i;
            }
        }
    }
    for (int i = 2; i <= MAX_VALUE; ++i)
    {
        addrCache[i] = i / factorCache[i];
    }
}

void factorize(int value, int* addrCache, int* factorCache, std::vector<int>& answer)
{
    assert(value >= 2 && value <= MAX_VALUE);
    answer.clear();

    while(factorCache[value])
    {
        answer.push_back(factorCache[value]);
        value = addrCache[value];
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

    int* addrCache = new int[MAX_VALUE + 1];
    int* factorCache = new int[MAX_VALUE + 1];
    buildSieve(addrCache, factorCache);

    std::size_t caseNumber;
    cin >> caseNumber;
    std::vector<int> answer;
    for (std::size_t i = 0; i < caseNumber; ++i)
    {
        int value;
        cin >> value;
        factorize(value, addrCache, factorCache, answer);
        for (auto it = answer.crbegin(); it < answer.crend(); ++it)
        {
            std::cout << *it << ' ';
        }
        std::cout << '\n';
    }

    delete[] addrCache;
    delete[] factorCache;

    return 0;
}
