/**
 * A. Grasshoper and Money
 *
 * input input.txt
 * output output.txt
 *
 * The grasshopper is traveling from cell 1 to cell n. At the beginning, the grasshopper sits on cell 1.
 * He can move from 1 to k cells forward in one jump.
 * In each cell grasshopper can get or lose several gold coins (for each cell this number is known).
 * Determine how the grasshopper needs to jump to maximize the total number of coins in the end.
 * Consider that the grasshopper cannot jump backwards.
 *
 * Input
 * The first line contains two integers: n and k (2 ≤ n, k ≤ 10000).
 * The second line contains n - 2 integers, the number of coins that the grasshopper gets on each cell, from the 2nd to the n - 1-th.
 * If this number is negative, grasshopper loses coins.
 * All the numbers do not exceed 10 000 by absolute value.
 *
 * Output
 * In the first line output the maximal number of coins.
 * In the second line output the number of jumps.
 * In the third line output the cells visited by the grasshoper.
 */

#include <iostream>
#include <fstream>
#include <vector>

//#define DEBUG

std::pair<int, std::vector<unsigned>> calcMaxCostForwardPath(const std::vector<int>& pointValues, unsigned maxStep)
{
    const unsigned pointCount = pointValues.size();
    std::vector<int> costs(pointCount);
    costs[0] = pointValues[0];
    std::vector<bool> visited(pointCount);
    std::vector<unsigned> backprop(pointCount);
    backprop[0] = pointCount;
    for (unsigned i = 0; i < pointCount; ++i)
    {
        const int pointCost = costs[i];
        for (unsigned j = 1, jLimit = maxStep > pointCount - i - 1 ? pointCount - i - 1 : maxStep; j <= jLimit; ++j)
        {
            const unsigned coord = i + j;
            const int newCost = pointValues[coord] + pointCost;
            if (!visited[coord])
            {
                costs[coord] = newCost;
                backprop[coord] = i;
                visited[coord] = true;
            }
            else if (costs[coord] < newCost)
            {
                costs[coord] = newCost;
                backprop[coord] = i;
            }
        }
    }

    std::vector<unsigned> backwardPath;
    for (unsigned i = pointCount - 1; i != pointCount; i = backprop[i])
    {
        backwardPath.push_back(i);
    }

    return std::make_pair(costs[pointCount - 1], backwardPath);
}

int main(void) {
#ifdef DEBUG
    std::ifstream cin("input.txt");
    using std::cout;
#else
    std::ifstream cin("input.txt");
    std::ofstream cout("output.txt");
#endif

    unsigned pointCount, maxStep;
    cin >> pointCount >> maxStep;
    std::vector<int> pointValues(pointCount);
    pointValues[0] = 0;
    pointValues[pointCount - 1] = 0;
    for (unsigned i = 1; i < pointCount - 1; ++i)
    {
        cin >> pointValues[i];
    }

    auto answer = calcMaxCostForwardPath(pointValues, maxStep);

    cout << answer.first << '\n' << (answer.second.size() - 1) << '\n';
    for (auto it = answer.second.rbegin(); it < answer.second.rend(); ++it)
    {
        cout << (*it + 1) << ' ';
    }
    cout << std::endl;
}
