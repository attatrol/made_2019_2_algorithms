/**
  * B. Множественный поиск
  *
  * ввод стандартный ввод
  * вывод стандартный вывод
  *
  * Дан массив строк s i и строка t.
  * Требуется для каждой строки s i определить, встречается ли она в t как подстрока.
  *
  * Входные данные
  * Первая строка входного файла содержит целое число n — число элементов в s (1 ≤ n ≤ 106).
  * Следующие n строк содержат по одной строке s i.
  * Сумма длин всех строк из s не превосходит 10^6.
  * Последняя строка входного файла содержит t (1 ≤ t ≤ 10^6).
  * Все строки состоят из строчных латинских букв.
  *
  * Выходные данные
  * Для каждой строки s i выведите «YES», если она встречается в t и «NO» в противном случае.
  * Строки нумеруются в порядке появления во входном файле.
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

static const char ALPHABET_START_CHAR = 'a';
static const std::size_t ALPHABET_SIZE = 26;
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

    std::size_t stringCount;
    cin >> stringCount;
    std::vector<std::string> strings(stringCount);
    for (std::size_t i = 0; i < stringCount; ++i)
    {
        cin >> strings[i];
    }
    std::string text;
    cin >> text;
    const std::size_t textLength = text.length();
    auto suffixArray = buildSuffixArray(text);

    for (const std::string& string : strings)
    {
#ifdef DEBUG
            std::cout << "\nFind " << string << " in " << text << '\n';
#endif
        std::size_t l = 0, r = textLength;
        while(r - l > 1)
        {
            std::size_t d = (r + l) / 2;
            std::string substring = text.substr(suffixArray[d], string.length());
#ifdef DEBUG
            std::cout << substring << '\n';
#endif
            if(substring <= string)
            {
                l = d;
            }
            else
            {
                r = d;
            }
        }
#ifdef DEBUG
        std::cout << text.substr(suffixArray[l], string.length()) << '\n';
#endif
        std::cout << (text.substr(suffixArray[l], string.length()) == string ? "YES\n" : "NO\n");
    }

    return 0;
}
