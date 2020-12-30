/**
 * D. Наибольшая общая подстрока
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Найдите наибольшую общую подстроку строк s и t.
 *
 * Входные данные
 * Первая строка входного файла содержит строку s, вторая — t (1 ≤ |s|, |t| ≤ 100000).
 * Строки состоят из строчных латинских букв.
 *
 * Выходные данные
 * Выведите одну строку — наибольшую общую подстроку строк s и t.
 * В случае, если ответ не единственный, выведите минимальный лексикографически.
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

static const char ALPHABET_START_CHAR = 'a' - 2;
static const std::size_t ALPHABET_SIZE = 28;
static const char DELIMITER_1 = ALPHABET_START_CHAR;
static const char DELIMITER_2 = ALPHABET_START_CHAR + 1;
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
        std::cout << std::setw(10) << result[i] + 1 << ' ' << inputTest.substr(result[i]) << '\n';
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

    std::string input1, input2;
    cin >> input1 >> input2;
    const std::string input = input1 + DELIMITER_1 + input2 + DELIMITER_2;
    auto suffixArray = buildSuffixArray(input);
    auto lcp = buildLCP(input, suffixArray);

    std::vector<bool> belongsToInput1(input.size());

    std::size_t commonSubstringIndex = input.length();
    std::size_t commonSubstringLength = 0;
    for (std::size_t i = 1, length = input.length(), length2 = input2.length() + 1; i < length; ++i)
    {
        const std::size_t suffixLength1 = suffixArray[i - 1];
        const std::size_t suffixLength2 = suffixArray[i];
        const bool belongsTo21 = length - suffixLength1 <= length2;
        const bool belongsTo22 = length - suffixLength2 <= length2;
#ifdef DEBUG
        std::cout << input.substr(suffixLength1) << " " << input.substr(suffixLength2) << '\n';
#endif
        if (belongsTo21 == belongsTo22)
        {
            continue;
        }
        const std::size_t lcpValue = lcp[i - 1];
        if (lcpValue > commonSubstringLength)
        {
            commonSubstringLength = lcpValue;
            commonSubstringIndex = i;
        }
    }

    if (commonSubstringLength)
    {
        std::cout << input.substr(suffixArray[commonSubstringIndex], commonSubstringLength);
    }
    else
    {
        std::cout << "";
    }

    return 0;
}
