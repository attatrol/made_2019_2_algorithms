/**
 * B. Removing brackets 2.0
 *
 * input standard input
 * output standard output
 *
 * Given a string made up of round, square and curly brackets.
 * Find the minimum number of characters that should be deleted so that the remaining characters form the correct bracket sequence
 *
 * Input
 * The input contains a string of round, square and curly brackets.
 * The length of the string does not exceed {100} characters.
 *
 * Output
 * Print the string with the maximum length that is a correct bracket sequence, and can be obtained from the original string by deleting some characters.
 */

//#define DEBUG

#include <assert.h>
#include <algorithm>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <queue>
#include <vector>

std::string pruneBadParenthesis(const std::string& input)
{

    struct state_t
    {
        std::size_t pos0_;
        std::size_t pos1_;
        std::size_t pruned_;
    };

    const std::size_t size = input.size();

    std::vector<state_t> dp(size * (size + 1)); // substring start X substring length

    for (std::size_t len = 0; len < size + 1; ++len)
    {
        for (std::size_t start = 0; start < size; ++start)
        {
            dp[start * (size + 1) + len].pruned_ = len;
#ifdef DEBUG
            dp[start * (size + 1) + len].pos0_ = dp[start * (size + 1) + len].pos1_ = 0;
#endif
        }
    }

    for (std::size_t len = 2; len < size + 1; ++len)
    {
        for (std::size_t start = 0; start <= size - len; ++start)
        {
            std::size_t minPrunedCount = len;
            const std::size_t coord = start * (size + 1) + len;
            for (std::size_t pos0 = start; pos0 <= start + len - 2; ++pos0)
            {
                for (std::size_t pos1 = pos0 + 1; pos1 <= start + len - 1; ++pos1)
                {
                    assert(pos0 < size && pos1 < size);
                    const char open = input[pos0];
                    const char close  = input[pos1];
                    if (open == '[' && close == ']'
                     || open == '(' && close == ')'
                     || open == '{' && close == '}')
                    {
                        // use recursive decomposition: prefix(0)1 -> |prefix| + 0 + 1
                        const std::size_t prefixLen = pos0 - start;
                        const std::size_t coord0 = (pos0 + 1) * (size + 1) + pos1 - pos0 - 1;
                        assert(coord0 < size * (size + 1));
                        std::size_t pruned = prefixLen + dp[coord0].pruned_;
                        if (pos1 + 1 < size)
                        {
                            const std::size_t coord1 = (pos1 + 1) * (size + 1) + len + start - pos1 - 1;
                            assert(coord1 < size * (size + 1));
                            pruned += dp[coord1].pruned_;
                        }
                        if (pruned < minPrunedCount)
                        {
                            dp[coord].pos0_ = pos0;
                            dp[coord].pos1_ = pos1;
                            minPrunedCount = pruned;
                        }
                    }
                }
            }
            dp[coord].pruned_ = minPrunedCount;
        }
    }

    // form result
    std::vector<bool> mask(size);
    using coord_t = std::pair<std::size_t, std::size_t>;
    std::queue<coord_t> queue;
    queue.emplace(0, size);
    while (queue.size())
    {
        coord_t coord = queue.front();
        queue.pop();
        assert(coord.first * (size + 1) + coord.second < (size +1) * size);
        const state_t cell = dp[coord.first * (size + 1) + coord.second];
        if (cell.pruned_ == coord.second)
        {
#ifdef DEBUG
            assert(cell.pos0_ == 0 && cell.pos1_ == 0);
#endif
            continue;
        }
        std::size_t pos0 = cell.pos0_;
        std::size_t pos1 = cell.pos1_;

        assert(pos0 < pos1);
        assert(!mask[pos0]);
        assert(!mask[pos1]);
        mask[pos0] = mask[pos1] = true;
        queue.emplace(pos0 + 1, pos1 - pos0 - 1);
        if (pos1 + 1 < size)
        {
            queue.emplace(pos1 + 1, coord.first + coord.second - pos1 - 1);
        }
    }

    std::string result = "";
    for (std::size_t i = 0; i < size; ++i)
    {
        if (mask[i])
        {
            result += input[i];
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
#endif

    std::string input;
    cin >> input;
    std::cout << pruneBadParenthesis(input);

    return 0;
}
