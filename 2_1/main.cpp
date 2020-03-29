/**
 * A. Разбиение на пары
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Однажды n ребят решили поучаствовать в VK Cup.
 * Для этого им нужно разбиться на команды по два человека.
 * Однако выяснилось, что не все ребята хорошо ладят друг с другом.
 * Вы выяснили для каждой пары ребят, могут ли они участвовать в одной команде.
 * Помогите им сформировать максимальное число команд.
 *
 * Входные данные
 * Первая строка содержит число n (1 ≤ n ≤ 18), далее n строк по n символов описывают отношения ребят.
 * Символ j строки i равен «Y», если ребята i и j могут участвовать в команде.
 *
 * Выходные данные
 * Выведите одно число — максимальное число участников в командах, которые можно сформировать.
 */

//#define DEBUG

#include <algorithm>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif

std::size_t calcIndependentEdgeSetPower(bool* graph, std::size_t vertexCount)
{
    std::size_t* dp = new std::size_t[1 << vertexCount];
    dp[0] = 0;
    for (std::size_t mask = 1; mask < 1 << vertexCount; ++mask)
    {
        for (std::size_t i = 0; i < vertexCount; ++i)
        {
            if ((mask >> i) & 1)
            {
                dp[mask] = dp[mask - (1 << i)];
                for (std::size_t j = 0; j < vertexCount; ++j)
                {
                    if ((((mask >> j) & 1) == 1) && graph[i * vertexCount + j])
                    {
                        dp[mask] = std::max(dp[mask], dp[mask - (1 << i) - (1 << j)] + 1);
                    }
                }
            }
        }
    }

    std::size_t result = dp[(1 << vertexCount) - 1];
    delete[] dp;
    return result;
}

int main()
{
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
#endif

    std::size_t vertexCount;
    cin >> vertexCount;
    bool* graph = new bool[vertexCount * vertexCount];
    for (std::size_t i = 0; i < vertexCount * vertexCount; ++i)
    {
        char edge;
        cin >> edge;
        graph[i] = edge == 'Y';
    }

    std::size_t playersCount = 2 * calcIndependentEdgeSetPower(graph, vertexCount);
    std::cout << playersCount;

    delete[] graph;

    return 0;
}
