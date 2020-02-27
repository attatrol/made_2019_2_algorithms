/**
 * B. Turtle and Money
 *
 * input standard input
 * output standard output
 *
 * There is a rectangular field of size n × m.
 * The turtle wants to move from cell (1, 1) to cell (n, m), in one step she can move to the next cell down or right.
 * For each passed cell, the turtle gains (or loses) several gold coins (this number is known for each cell).
 * Determine what the maximum number of coins Turtle can collect on the way and how she needs to go for it.
 *
 * Input
 * The first line contains two integers: n and m (2 ≤ n, m ≤ 1000).
 * Each of the following n lines contains m numbers aij (|aij| ≤ 10), which indicate the number of coins received by the turtle on each cell.
 * If this number is negative, the turtle loses coins.
 *
 * Output
 * In the first line, output maximal number of coins that turtle can collect.
 * In the second output the commands to be executed by the turtle, without spaces : the letter «R» indicates a step to the right, and the letter «D» a step down.
 */

#include <assert.h>
#include <iostream>
#include <fstream>
#include <vector>

//#define DEBUG
#define NDEBUG

static const char MOVE_RIGHT = 'R';
static const char MOVE_DOWN  = 'D';

std::pair<int, std::vector<char>> calcMaxCostPath(const std::vector<int>& pointValues, const unsigned rowCount, const unsigned colCount)
{
    const unsigned pointCount = pointValues.size();
    const int impossiblyLowCost = -10 * (colCount + rowCount);
    std::vector<int> costs(pointCount, impossiblyLowCost);
    std::vector<char> steps(pointCount);

    auto step = [&](const unsigned fromRowIdx, const unsigned fromColIdx, const unsigned toRowIdx, const unsigned toColIdx)
    {
        assert(fromRowIdx + fromColIdx + 1 == toRowIdx + toColIdx);
        const unsigned fromCoord = fromRowIdx * colCount + fromColIdx;
        const unsigned toCoord   = toRowIdx   * colCount + toColIdx;
        assert(fromCoord < rowCount * colCount);
        assert(toCoord   < rowCount * colCount);

        const int newCost = pointValues[toCoord] + costs[fromCoord];
//#ifdef DEBUG
//            std::cout << '[' << fromRowIdx << "; " << fromColIdx << "] -> [" << toRowIdx << "; " << toColIdx  << "] old cost: " << costs[toCoord] << " new cost: " << newCost << '\n';
//#endif
        if (costs[toCoord] < newCost)
        {
            costs[toCoord] = newCost;
            steps[toCoord] = fromRowIdx == toRowIdx ? MOVE_RIGHT : MOVE_DOWN;
        }
    };

    // first step
    costs[0] = pointValues[0];

    // traverse point field by every diagonal secant:
    for (unsigned secantIdx = 1; secantIdx < rowCount + colCount - 1; ++secantIdx)
    {
#ifdef DEBUG
        std::cout << "secantIdx: " << secantIdx << '\n';
#endif
        const unsigned secantOffset = secantIdx < colCount ? 0 : secantIdx - colCount + 1;
        unsigned secantEnd = secantIdx - secantOffset;
        unsigned secantStart = secantIdx < rowCount ? 0 : secantIdx - rowCount + 1;
        if (secantIdx == secantEnd)
        {
#ifdef DEBUG
        std::cout << '[' << secantEnd << "; " << secantIdx - secantEnd << "]\n";
#endif
            step(secantIdx - secantEnd, secantEnd - 1, secantIdx - secantEnd, secantEnd);
            secantEnd--;
        }
        if (!secantStart)
        {
#ifdef DEBUG
        std::cout << '[' << secantStart << "; " << secantIdx - secantStart << "]\n";
#endif
            step(secantIdx - secantStart - 1, secantStart, secantIdx - secantStart, secantStart);
            secantStart++;
        }

        for (unsigned i = secantStart; i <= secantEnd; ++i)
        {
#ifdef DEBUG
        std::cout << '[' << i << "; " << secantIdx - i << "]\n";
#endif
            step(secantIdx - i - 1, i,     secantIdx - i, i);
            step(secantIdx - i,     i - 1, secantIdx - i, i);
        }
    }

    // prepare answer
    std::vector<char> maxPath(rowCount + colCount - 2);
    for (unsigned i = rowCount + colCount - 3, coord = rowCount * colCount - 1; i < rowCount + colCount - 1; --i)
    {
        const char step = steps[coord];
        maxPath[i] = step;
        coord -= (step == MOVE_RIGHT ? 1 : colCount);
        assert(coord < rowCount * colCount);
    }

    return std::make_pair(costs[pointCount - 1], maxPath);
}

int main(void) {
#ifdef DEBUG
    std::ifstream cin("input.txt");
    using std::cout;
#else
    using std::cin;
    using std::cout;
#endif

    unsigned rowCount, colCount;
    cin >> rowCount >> colCount;
    std::vector<int> pointValues(rowCount * colCount);
    for (unsigned i = 0; i < rowCount * colCount; ++i)
    {
        cin >> pointValues[i];
    }

    auto answer = calcMaxCostPath(pointValues, rowCount, colCount);

    cout << answer.first << '\n';
    for (auto& step : answer.second)
    {
        cout << step;
    }
    cout << std::endl;
}
