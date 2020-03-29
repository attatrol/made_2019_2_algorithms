/**
 * F. Дубы
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * На аллее перед зданием Министерства Обороны в ряд высажены n дубов.
 * В связи с грядущим приездом главнокомандующего, было принято решение срубить несколько деревьев для придания аллее более милитаристического вида.
 * Внутренние распорядки министерства позволяют срубать дуб только в двух случаях:
 *     если и ближайший дуб слева, и ближайший дуб справа строго ниже, чем данный дуб;
 *     если и ближайший дуб слева, и ближайший дуб справа строго выше, чем данный дуб.
 * В частности, согласно этому правилу, нельзя срубить крайний левый и крайний правый дубы.
 * Министр хочет выработать такой план вырубки, чтобы в итоге осталось несколько дубов, высоты которых образуют неубывающую последовательность, то есть чтобы каждый дуб был не ниже, чем все дубы, стоящие слева от него.
 * При этом, как человек любящий флору, министр хочет, чтобы было срублено минимальное возможное количество деревьев.
 * Помогите сотрудникам министерства составить оптимальный план вырубки аллеи или выяснить, что срубить дубы соответствующим образом невозможно.
 *
 * Входные данные
 * Первая строка входного файла содержит целое число n — количество дубов, растущих на аллее (2 ≤ n ≤ 200).
 * Вторая строка содержит n чисел — высоты дубов, приведенные слева направо.
 * Высоты дубов — положительные целые числа, не превышающие 1 000.
 *
 * Выходные данные
 * Если оставить последовательность дубов с неубывающими высотами невозможно, выходной файл должен содержать только одно число  - 1.
 * В случае, если искомый план существует, в первую строку выходного файла выведите целое число m — минимальное количество дубов, которые необходимо срубить.
 * В следующие m строк выведите оптимальный план вырубки деревьев — номера дубов в том порядке, в котором их следует срубать, по одному номеру на строке.
 * Дубы нумеруются слева направо натуральными числами от 1 до n.
 * Если планов с наименьшим числом срубаемых дубов несколько, выведите любой из них.
 */

//#define DEBUG

#include <assert.h>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>

using unit_t = unsigned int;

struct state_t
{
    std::size_t maxDeletedCount_; // how much we can delete from interval
    std::size_t deletedCount_;    // actually deleted count
    std::size_t prunedIndex_;     // index of removed item
};

void extractDeletedIndex(const state_t* dp, const std::size_t size, const std::size_t start, const std::size_t end, std::vector<std::size_t>& answer) {
    const std::size_t coord = start * size + end;
    assert(dp[coord].maxDeletedCount_ < size + 1); // impossibleSize
    const std::size_t deletedCount = dp[coord].maxDeletedCount_;
    if (deletedCount == start) return;
    extractDeletedIndex(dp, size, start, deletedCount, answer);
    extractDeletedIndex(dp, size, deletedCount, end, answer);
    answer.push_back(deletedCount);
}

void extractAnswer(const state_t* dp, const std::size_t size, const std::size_t start, const std::size_t end, std::vector<std::size_t>& answer) {
    const std::size_t coord = start * size + end;
    assert(dp[coord].prunedIndex_ < size + 1);
    if (dp[coord].prunedIndex_ == start) {
        extractDeletedIndex(dp, size, start, end, answer);
        return;
    }
    extractAnswer(dp, size, start, dp[coord].prunedIndex_, answer);
    extractAnswer(dp, size, dp[coord].prunedIndex_, end, answer);
}

constexpr bool canBePruned(unit_t value, unit_t neighbour0Value, unit_t neighbour1Value)
{
    return (value < neighbour0Value) && (value < neighbour1Value) || (value > neighbour0Value) && (value > neighbour1Value);
}

#ifdef DEBUG
void print(state_t* dp, std::size_t size)
{
    for (std::size_t i = 0; i < size; ++i)
    {
        for (std::size_t j = 0; j < size; ++j)
        {
            const state_t* cell = dp + i * size + j;
            std::cout << cell->maxDeletedCount_ << " " << cell->deletedCount_ << " " << cell->prunedIndex_ << " | ";
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}
#endif

std::pair<bool, std::vector<std::size_t>> pruneSequenceIntoMonotonic(const std::vector<unit_t>& sequence)
{
    const std::size_t size = sequence.size();
    const std::size_t impossibleIndex = size + 1;
    const std::size_t impossibleSize = size + 1;

    state_t* dp = new state_t[size * size];

    for (std::size_t i = 0; i < size * size; ++i)
    {
        dp[i].maxDeletedCount_ = impossibleSize;
        dp[i].deletedCount_ = impossibleSize;
        dp[i].prunedIndex_ = impossibleIndex;
    }

    // initialize maxDeletedCount_
    for (std::size_t i = 0; i < size - 1; ++i)
    {
        assert(i * size + i + 1 < size * size);
        dp[i * size + i + 1].maxDeletedCount_ = i;
    }

    // calc maxDeletedCount_
    for (std::size_t len = 2; len < size; ++len)
    {
        for (std::size_t start = 0; start < size - len; ++start)
        {
            const std::size_t end = start + len;
            const std::size_t coord = start * size + end;
            assert(coord < size * size);
            for (std::size_t i = start + 1; i < end; ++i)
            {
                const std::size_t leftCoord = start * size + i;
                assert(leftCoord < size * size);
                const std::size_t rightCoord = i * size + end;
                assert(rightCoord < size * size);
                if (dp[leftCoord].maxDeletedCount_ != impossibleSize
                        && dp[rightCoord].maxDeletedCount_ != impossibleSize
                        && canBePruned(sequence[i], sequence[start], sequence[end]))
                {
                    dp[coord].maxDeletedCount_ = i;
                    break;
                }
            }
        }
    }

    // init deletedCount_
    for (std::size_t len = 1; len < size; ++len)
    {
        for (std::size_t start = 0; start < size - len; ++start)
        {
            const std::size_t end = start + len;
            const std::size_t coord = start * size + end;
            assert(coord < size * size);
            if (sequence[start] > sequence[end])
            {
                continue;
            }
            if (dp[coord].maxDeletedCount_ != impossibleSize)
            {
                dp[coord].deletedCount_ = end - start - 1;
                dp[coord].prunedIndex_ = start;
            }
        }
    }

    // calc deletedCount_
    for (std::size_t len = 1; len < size; ++len)
    {
        for (std::size_t start = 0; start < size - len; ++start)
        {
            const std::size_t end = start + len;
            const std::size_t coord = start * size + end;
            assert(coord < size * size);

            std::size_t minDeletedCount = dp[coord].deletedCount_;
            for (std::size_t i = start + 1; i < end; i++)
            {
                if (sequence[i] < sequence[start] || sequence[i] > sequence[end])
                {
                    continue;
                }
                const std::size_t leftCoord = start * size + i;
                assert(leftCoord < size * size);
                const std::size_t rightCoord = i * size + end;
                assert(rightCoord < size * size);
                const std::size_t deletedCount = dp[leftCoord].deletedCount_ + dp[rightCoord].deletedCount_;
                if (minDeletedCount > deletedCount)
                {
                    minDeletedCount = deletedCount;
                    dp[coord].prunedIndex_ = i;
                }
            }
            dp[coord].deletedCount_ = minDeletedCount;
        }
    }
#ifdef DEBUG
    print(dp, size);
#endif

    // prepare answer
    if (dp[size - 1].prunedIndex_ == impossibleIndex)
    {
        delete[] dp;
        return std::make_pair(false, std::vector<std::size_t>());
    }
    else
    {
        std::vector<std::size_t> answer;
        extractAnswer(dp, size, 0, size - 1, answer);
        delete[] dp;
        return std::make_pair(true, answer);
    }
}


int main()
{
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
#endif

    std::size_t count; // n rows and m columns
    cin >> count;
    std::vector<unit_t> heights(count);
    for (std::size_t i = 0; i < count; ++i)
    {
        cin >> heights[i];
    }

    auto answer = pruneSequenceIntoMonotonic(heights);

    if (answer.first)
    {
        std::cout << answer.second.size() << '\n';
        for (auto& value: answer.second)
        {
            std::cout << (value + 1) << '\n';
        }
    }
    else
    {
        std::cout << "-1\n";
    }

    return 0;
}
