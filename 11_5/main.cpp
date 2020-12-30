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

inline std::pair<std::size_t, std::size_t> calcHighPow2(std::size_t value)
{
    std::size_t pow2 = 1, exponent = 1;
    while(pow2 <= value)
    {
        pow2 <<= 1;
        exponent += 1;
    }
    return std::make_pair(pow2, exponent);
}

static const char ALPHABET_START_CHAR = static_cast<char>(32);
static const std::size_t ALPHABET_SIZE = 126 - 32 + 1;
static const char PLACEHOLDER_CHAR = ALPHABET_START_CHAR - 1;

std::vector<std::size_t> buildSuffixArray(const std::string& input)
{

    const std::size_t length = input.length();
    const auto [size, roundCount] = calcHighPow2(length);
    const std::size_t mask = size - 1;
#ifdef DEBUG
        std::cout << "length: " << length
                  << " | size: " << size
                  << " | roundCount: " << roundCount << '\n';
#endif

#ifdef DEBUG
        std::string inputTest = input;
        for (std::size_t j = length; j < size; ++j)
        {
            inputTest += PLACEHOLDER_CHAR;
        }
#endif

    std::size_t* clsBuckets = new std::size_t[std::max(ALPHABET_SIZE + 1, size) + 1];
    std::size_t* clsIndexMap = new std::size_t[ALPHABET_SIZE + 1];
    std::size_t clsCount = ALPHABET_SIZE + 1;

    // bucket sort - first iteration for single letters
    std::fill(clsBuckets, clsBuckets + clsCount, 0);
    for(std::size_t i = 0; i < length; ++i)
    {
        ++clsBuckets[input[i] - PLACEHOLDER_CHAR];
    }
    clsBuckets[0] = size - length;
    std::size_t clsIndex = 0;
    for(std::size_t i = 0; i < clsCount; ++i)
    {
        if (clsBuckets[i])
        {
            clsIndexMap[i] = clsIndex;
            ++clsIndex;
        }
    }
    for(std::size_t i = 1; i < clsCount; ++i)
    {
        clsBuckets[i] += clsBuckets[i - 1];
    }
    clsCount = clsIndex;
    std::size_t* sortedPos = new std::size_t[size];
    std::size_t* cls = new std::size_t[size];
    for(std::size_t i = 0; i < length; ++i)
    {
        sortedPos[i] = clsBuckets[input[i] - PLACEHOLDER_CHAR - 1]++;
        cls[i] = clsIndexMap[input[i] - PLACEHOLDER_CHAR];
#ifdef DEBUG
        std::cout << input[i] << " | " << std::setw(10) << sortedPos[i] << " | " << std::setw(10) << cls[i] << '\n';
#endif
    }
    for(std::size_t i = 0; i < size - length; ++i)
    {
        sortedPos[i + length] = i;
        cls[i + length] = 0;
#ifdef DEBUG
        std::cout << PLACEHOLDER_CHAR << " | " << std::setw(10) << sortedPos[i + length] << " | " << std::setw(10) << cls[i + length] << '\n';
#endif
    }

    // bucket sort - every next iteration

    std::size_t* cls2 = new std::size_t[size];
    std::size_t* sortedPos2 = new std::size_t[size];

    for (std::size_t i = 0; i < roundCount - 1; ++i)
    {
#ifdef DEBUG
        std::cout << "ITERATION " << i << '\n';
#endif
        const std::size_t shift = 1 << i;
        assert(shift < size);

        std::fill(clsBuckets, clsBuckets + clsCount + 1, 0);
        for (std::size_t j = 0; j < size; ++j)
        {
            assert(cls[(j + shift) & mask] < size);
            ++clsBuckets[cls[(j + shift) & mask] + 1];
        }
        for(std::size_t j = 1; j < clsCount + 1; ++j)
        {
            clsBuckets[j] += clsBuckets[j - 1];
        }
        for(std::size_t j = 0; j < size; ++j)
        {
            assert(cls[(j + shift) & mask] < size);
            sortedPos[clsBuckets[cls[(j + shift) & mask]]++] = j;
        }
#ifdef DEBUG
        std::cout << "\nj ( leftEquiCls[j] ;  rightEquiCls[j]) | sortedPos[j] \n";
        for (std::size_t j = 0; j < size; ++j)
        {
            std::cout << std::setw(10) <<j <<" (" << std::setw(10) << cls[j] << "; " << std::setw(10) << cls[(j + shift) & mask] << ") | "
                      << std::setw(10) << sortedPos[j] << '\n';
        }
#endif

        std::fill(clsBuckets, clsBuckets + clsCount + 1, 0);
        for (std::size_t j = 0; j < size; ++j)
        {
            assert(cls[j] < size);
            ++clsBuckets[cls[j] + 1];
        }
        for(std::size_t j = 1; j < clsCount + 1; ++j)
        {
            clsBuckets[j] += clsBuckets[j - 1];
        }
        for(std::size_t j = 0; j < size; ++j)
        {
            assert(sortedPos[j] < size);
            assert(cls[sortedPos[j]] < size);
            sortedPos2[sortedPos[j]] = clsBuckets[cls[sortedPos[j]]]++;
        }
#ifdef DEBUG
        std::fill(cls2, cls2 + size, 42);
        std::cout << "sortedPos2[0] " << sortedPos2[0] << std::endl;
#endif
        for(std::size_t j = 0; j < size; ++j)
        {
            sortedPos[sortedPos2[j]]  = j;
        }
        std::size_t clsIndex = 0;
        cls2[sortedPos[0]] = 0;
        std::size_t leftClsIndex = cls[sortedPos[0]];
        std::size_t rightClsIndex = cls[(sortedPos[0] + shift) & mask];
        for (std::size_t j = 1; j < size; ++j)
        {
            if (leftClsIndex != cls[sortedPos[j]] || rightClsIndex != cls[(sortedPos[j] + shift) & mask])
            {
                leftClsIndex = cls[sortedPos[j]];
                rightClsIndex = cls[(sortedPos[j] + shift) & mask];
                ++clsIndex;
            }
            cls2[sortedPos[j]] = clsIndex;
        }
        std::swap(cls, cls2);
        clsCount = clsIndex + 1;
#ifdef DEBUG
        std::cout << "\n( leftEquiCls[j] ;  rightEquiCls[j]) | sortedPos1[j] | equiCls[j] \n";
        for (std::size_t j = 0; j < size; ++j)
        {
            std::cout << "(" << std::setw(10) << cls2[j] << "; " << std::setw(10) << cls[(j + shift) & mask] << ") | "
                      << std::setw(10) << sortedPos2[j] << " | " << std::setw(10) << cls[j] << '\n';
        }
#endif
    }

    std::vector<std::size_t> result(input.length());

    for (std::size_t i = 0; i < size; ++i)
    {
        if (sortedPos2[i] >= size - input.length())
        {
            result[sortedPos2[i] - size + input.length()] = i;
        }
    }
#ifdef DEBUG
    for (std::size_t i = 0; i < input.length(); ++i)
    {
        std::cout << std::setw(10) << i << std::setw(10) << result[i] + 1 << ' ' << inputTest.substr(result[i]) << '\n';
    }
#endif

    delete[] clsBuckets;
    delete[] clsIndexMap;
    delete[] sortedPos;
    delete[] cls;
    delete[] sortedPos2;
    delete[] cls2;

    return result;
}

std::vector<std::size_t> buildLCP(const std::string& input, const std::vector<std::size_t>& suffixArray)
{
    const std::size_t length = input.length();
    std::vector<std::size_t> result(length);
    std::vector<std::size_t> pos(length);
    for (std::size_t i = 0; i < length; ++i)
    {
       pos[suffixArray[i]] = i;
    }

    std::size_t curr = 0;
    for (std::size_t i = 0; i < length; ++i)
    {
        if (curr > 0)
        {
            --curr;
        }
        if (pos[i] == length - 1)
        {
            result[length - 1] = 0;
            curr = 0;
        }
        else
        {
            std::size_t j = suffixArray[pos[i] + 1];
            while (std::max(i + curr, j + curr) < length && input[i + curr] == input[j + curr])
            {
                ++curr;
            }
            result[pos[i]] = curr;
        }
    }

    return result;
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

    std::string input;
    std::size_t inputCycleIndex;
    cin >> input >> inputCycleIndex;
    --inputCycleIndex;
    input = input + input;

    if (input.length() == 0)
    {
        // Есть ли у пустой строки хотя бы одно вращение?
        std::cout << (inputCycleIndex == 0 ? "\n" : "IMPOSSIBLE\n");
        return 0;
    }

    auto suffixArray = buildSuffixArray(input);
    auto lcp = buildLCP(input, suffixArray);

    for (std::size_t i = 0, cycleIndex = 0, length = input.length(); i < length; ++i)
    {
        const std::size_t lcpValue = lcp[i];
        const std::size_t suffixLength = length - suffixArray[i];
        if(lcpValue < suffixLength)
        {
            ++cycleIndex;
        }
        if (cycleIndex == inputCycleIndex + 1)
        {
            std::cout << input.substr(suffixArray[i], length / 2) << '\n';
            return 0;
        }
    }

    std::cout << "IMPOSSIBLE\n";
    return 0;
}
