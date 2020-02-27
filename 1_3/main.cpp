/**
 * C. Наибольшая возрастающая подпоследовательность
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Пусть a1, a2, ..., an — числовая последовательность.
 * Длина последовательности — это количество элементов этой последовательности.
 * Последовательность ai1, ai2, ..., aik называется подпоследовательностью последовательности a, если 1 ≤ i1 < i2 < ... < ik ≤ n.
 * Последовательность a называется возрастающей, если a1 < a2 < ... < an.
 * Вам дана последовательность, содержащая n целых чисел.
 * Найдите ее самую длинную возрастающую подпоследовательность.
 *
 * Входные данные
 * В первой строке задано одно число n (1 ≤ n ≤ 2000) — длина подпоследовательности.
 * В следующей строке задано n целых чисел ai ( - 109 ≤ ai ≤ 109) — элементы последовательности.
 *
 * Выходные данные
 * В первой строке выведите число k — длину наибольшей возрастающей подпоследовательности.
 * В следующей строке выведите k чисел — саму подпоследовательность.
 */

#define DEBUG

#include <algorithm>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>


std::vector<int> calcMaxSubsequence(const std::vector<int>& sequence)
{
    const std::size_t sequenceLength = sequence.size();
    std::vector<std::size_t> maxSubsequenceLengths(sequenceLength, 1);
    std::vector<std::size_t> maxSubsequencePrededcessors(sequenceLength, sequenceLength);

    // find the subsequence of max length
    for (std::size_t i = 1; i < sequenceLength; ++i)
    {
        const int value = sequence[i];
        std::size_t foundMaxSubsequence = 0;
        std::size_t predecessor;
        for(std::size_t j = i - 1; j < i; --j)
        {
            if (value > sequence[j] && foundMaxSubsequence < maxSubsequenceLengths[j])
            {
                predecessor = j;
                foundMaxSubsequence = maxSubsequenceLengths[j];
            }
//            if (foundMaxSubsequence > j)
//            {
//                break;
//            }
        }
        if (foundMaxSubsequence)
        {
            maxSubsequencePrededcessors[i] = predecessor;
            maxSubsequenceLengths[i] = foundMaxSubsequence + 1;
        }
    }

    // prepare answer
    std::size_t maxSubsequenceLength = 1;
    std::size_t index = 0;
    for (std::size_t i = sequenceLength - 1; i > maxSubsequenceLength - 1 && i < sequenceLength; --i)
    {
        if (maxSubsequenceLengths[i] > maxSubsequenceLength)
        {
            maxSubsequenceLength = maxSubsequenceLengths[i];
            index = i;
        }
    }
    std::vector<int> maxSubsequence(maxSubsequenceLength);
    for (std::size_t i = maxSubsequenceLength - 1; index != sequenceLength; --i, index = maxSubsequencePrededcessors[index])
    {
        maxSubsequence[i] = sequence[index];
    }

    return maxSubsequence;
}

int main(void) {
#ifdef DEBUG
    std::ifstream cin("input.txt");
    using std::cout;
#else
    using std::cin;
    using std::cout;
#endif

    std::size_t sequenceLength;
    cin >> sequenceLength;
    std::vector<int> sequence(sequenceLength);
    for (std::size_t i = 0; i < sequenceLength; ++i)
    {
        cin >> sequence[i];
    }

    auto answer = calcMaxSubsequence(sequence);

    cout << answer.size() << '\n';
    for (auto& value : answer)
    {
        cout << value << ' ';
    }
    cout << std::endl;
}
