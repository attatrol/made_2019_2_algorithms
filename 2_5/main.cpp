/**
 * E. Domino Tiling
 *
 * input standard input
 * output standard output
 *
 * You are given a rectangular grid of square cells. The character 'X' represents a cell that is already covered, the character '.' is a cell that still needs to be covered.
 * You want to cover all the '.' cells using a collection of disjoint 2×1 dominos.
 * Return the number of ways to do this.
 * Two ways are considered different if two cells are covered by the same domino in one tiling and by two different dominos in the other tiling.
 *
 * Входные данные
 * Первая строка содержит числа n и m (1≤n,m≤12). Следующие n строк содержат по m символов — описание доски.
 * Выходные данные
 * Выведите число замощений доминошками.
 */

#define DEBUG

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <limits>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>

static const char BLOCKED_CELL = 'X';

unsigned long long calcTiling2x1Count(const std::vector<bool>& space, const std::size_t cols, const std::size_t rows)
{
    assert(cols >= rows);
    const std::size_t maskSize = 1 << rows;
    std::vector<unsigned long long> dp((rows * cols + 1) * maskSize);

    dp[0] = 1;

    for (std::size_t j = 0; j < cols; ++j)
    {
        for (std::size_t i = 0; i < rows; ++i)
        {
            const std::size_t spaceCoord = j * rows + i;
            assert(spaceCoord < rows * cols + 1);
            const std::size_t nextRowSpaceCoord = j * rows + i + 1;
            assert(nextRowSpaceCoord < rows * cols + 1);
            for (std::size_t mask = 0; mask < maskSize; ++mask)
            {
                const unsigned long long value = dp[spaceCoord * maskSize + mask];
                if ((mask & 1) || !space[spaceCoord]) // the target cell is occupied
                {
                    dp[nextRowSpaceCoord * maskSize + (mask >> 1)] += value;
                }
                else // the target cell is NOT occupied
                {
                    if (i != rows - 1 && (mask & 2) == 0 && space[nextRowSpaceCoord]) // NOT the last row && the cell next row is NOT occupied
                    {
                        dp[nextRowSpaceCoord * maskSize + (mask >> 1) + 1] += value; // vert domino
                    }
                    if (j != cols - 1) // NOT the last column
                    {
                        const std::size_t nextColSpaceCoord = (j + 1) * rows + i;
                        if (space[nextColSpaceCoord]) // the cell next col is NOT occupied
                        {
                            dp[nextRowSpaceCoord * maskSize + (mask >> 1) + (1 << (rows - 1))] += value; // horz domino
                        }
                    }
                }
            }
        }
    }

    return dp[rows * cols * maskSize + 0];
}


int main()
{
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
#endif

    std::size_t rows, cols; // n rows and m columns
    cin >> rows >> cols;

    std::vector<bool> space(rows * cols);

    if (cols >= rows)
    {
        for (std::size_t i = 0; i < rows; ++i)
        {
            for (std::size_t j = 0; j < cols; ++j)
            {
                char cell;
                cin >> cell;
                space[j * rows + i] = cell != BLOCKED_CELL;
            }
        }
    }
    else
    {
        for (std::size_t i = 0; i < rows; ++i)
        {
            for (std::size_t j = 0; j < cols; ++j)
            {
                char cell;
                cin >> cell;
                space[i * cols + j] = cell != BLOCKED_CELL;
            }
        }
        std::swap(rows, cols);
    }

    std::cout << calcTiling2x1Count(space, cols, rows);

    return 0;
}
