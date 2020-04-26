/**
 * C. Улиточки
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Две улиточки Маша и Петя сейчас находятся в на лужайке с абрикосами и хотят добраться до своего домика.
 * Лужайки пронумерованы числами от 1 до n и соединены дорожками (может быть несколько дорожек соединяющих две лужайки, могут быть дорожки, соединяющие лужайку с собой же).
 * В виду соображений гигиены, если по дорожке проползла улиточка, то вторая по той же дорожке уже ползти не может.
 * Помогите Пете и Маше добраться до домика.
 *
 * Входные данные
 * В первой строке файла записаны четыре целых числа — n, m, s и t (количество лужаек, количество дорог, номер лужайки с абрикосами и номер домика).
 * В следующих m строках записаны пары чисел.
 * Пара чисел (x, y) означает, что есть дорожка с лужайки x до лужайки y (из-за особенностей улиток и местности дорожки односторонние).
 * Ограничения: 2 <= n <= 10^5, 0 <= m <= 10^5, s != t.
 *
 * Выходные данные
 * Если существует решение, то выведите YES и на двух отдельных строчках сначала последовательность лужаек для Машеньки (дам нужно пропускать вперед), затем путь для Пети.
 * Если решения не существует, выведите NO.
 * Если решений несколько, выведите любое.
 */

#define DEBUG

#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <iomanip>
#include <algorithm>
#include <vector>
#include <limits>
#include <list>
#include <queue>
#include <optional>


struct Edge
{
    Edge* oppositeEdge;
    const std::size_t to;
    const std::size_t index;
    int throughput;
    int flow;

    Edge(std::size_t to, int throughput, std::size_t index)
        : to(to), index(index), throughput(throughput), flow(0)
    {
    }

    int getCurrentThroughput() const
    {
        return throughput - flow;
    }
};

class Network {
private:
    const std::size_t vertexCount_;
    const std::size_t edgeCount_;
    const std::size_t sourceIndex_;
    const std::size_t sinkIndex_;
    std::size_t edgeCounter_;
    std::vector<std::list<Edge>> edges_;

    std::pair<bool, std::vector<int>> bfs();
    int dfs(std::size_t vertexIndex, std::vector<std::list<Edge>::iterator>& block, std::vector<int>& level, int throughput = std::numeric_limits<int>::max());
    int dinic();
    std::optional<std::list<std::size_t>> extractPath();
public:
    Network(std::size_t vertexCount, std::size_t edgeCount, std::size_t sourceIndex, std::size_t sinkIndex)
        : vertexCount_(vertexCount), edgeCount_(edgeCount), sourceIndex_(sourceIndex), sinkIndex_(sinkIndex), edgeCounter_(0), edges_(vertexCount)
    {
    }
    void addEdge(std::size_t from, std::size_t to, int throughput = 1);
    void clear();
    std::optional<std::pair<std::list<std::size_t>, std::list<std::size_t>>> find2Paths();
};

void Network::addEdge(std::size_t from, std::size_t to, int throughput)
{
    edges_[from].emplace_back(to, throughput, edgeCounter_);
    edges_[to].emplace_back(from, 0, edgeCounter_);
    edges_[from].back().oppositeEdge = &edges_[to].back();
    edges_[to].back().oppositeEdge = &edges_[from].back();
    ++edgeCounter_;
}

void Network::clear()
{
    for (auto& row: edges_)
    {
        for (auto& edge: row)
        {
            edge.flow = 0;
        }
    }
}

std::pair<bool, std::vector<int> > Network::bfs()
{
    std::vector<int> level(vertexCount_, std::numeric_limits<int>::max());
    level[sourceIndex_] = 0;

    std::queue<std::size_t> queue;
    queue.push(sourceIndex_);

    while (!queue.empty())
    {
        std::size_t vertexIndex = queue.front();
        queue.pop();

        for (auto& edge : edges_[vertexIndex])
        {
            if (level[edge.to] == std::numeric_limits<int>::max() && edge.getCurrentThroughput() > 0)
            {
                level[edge.to] = level[vertexIndex] + 1;
                queue.push(edge.to);
            }
        }
    }

    return { level[sinkIndex_] != std::numeric_limits<int>::max(), std::move(level) };
}

int Network::dfs(std::size_t vertexIndex, std::vector<std::list<Edge>::iterator> &block, std::vector<int> &level, int throughput)
{
    if (vertexIndex == sinkIndex_)
    {
        return throughput;
    }
    if (!throughput)
    {
        return 0;
    }

    for (auto it = block[vertexIndex]; it != edges_[vertexIndex].end(); ++it)
    {
        if (level[it->to] != level[vertexIndex] + 1)
        {
            continue;
        }

        int flow_get = dfs(it->to, block, level, std::min(it->getCurrentThroughput(), throughput));
        if (flow_get)
        {
            it->flow += flow_get;
            it->oppositeEdge->flow -= flow_get;

            return flow_get;
        }
        ++block[vertexIndex];
    }

    return 0;
}

int Network::dinic()
{
    int result = 0;
    auto [incomplete, level] = bfs();
            while (incomplete)
    {
        std::vector<std::list<Edge>::iterator> block(vertexCount_);
        for (std::size_t i = 0; i < vertexCount_; ++i)
        {
            block[i] = edges_[i].begin();
        }

        int flow = dfs(sourceIndex_, block, level);
        while (flow)
        {
            result += flow;
            flow = dfs(sourceIndex_, block, level);
        }
        auto pr = bfs();
        incomplete = pr.first;
        level = pr.second;
    }
    return result;
}

std::optional<std::list<std::size_t> > Network::extractPath()
{
    if (!dinic())
    {
        return std::nullopt;
    }

    std::list<std::size_t> result{sourceIndex_};
    std::vector<bool> visited(edgeCount_);

    for (std::size_t vertexIndex = sourceIndex_; vertexIndex != sinkIndex_; )
    {
        std::size_t vertexIndex2 = vertexIndex;
        for (Edge& e: edges_[vertexIndex])
        {
            if (e.flow > 0 && !visited[e.index])
            {
                vertexIndex = e.to;
                visited[e.index] = true;
                result.push_back(vertexIndex);
                e.throughput = 0;
                break;
            }
        }
        if (vertexIndex == vertexIndex2)
        {
            return std::nullopt;
        }
    }
    clear();

    return std::make_optional(std::move(result));
}

std::optional<std::pair<std::list<std::size_t>, std::list<std::size_t> > > Network::find2Paths()
{
    auto first = extractPath();
    if (!first)
    {
        return std::nullopt;
    }

    auto second = extractPath();
    if (!second)
    {
        return std::nullopt;
    }

    return std::make_optional<std::pair<std::list<std::size_t>, std::list<std::size_t>>>(first.value(), second.value());
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

    std::size_t vertexCount, edgeCount, sourceIndex, sinkIndex;
    cin >> vertexCount >> edgeCount >> sourceIndex >> sinkIndex;

    Network network(vertexCount, edgeCount, sourceIndex - 1, sinkIndex - 1);
    for (std::size_t i = 0; i < edgeCount; ++i) {
        std::size_t from, to;
        cin >> from >> to;
        network.addEdge(from - 1, to - 1);
    }

    auto result = network.find2Paths();

    if (result) {
        std::cout << "YES\n";
        auto [first, second] = result.value();
        auto printn = [] (const int& n) { std::cout << n + 1 << ' '; };

        std::for_each(first.begin(), first.end(), printn);
        std::cout << '\n';
        std::for_each(second.begin(), second.end(), printn);
    } else {
        std::cout << "NO";
    }

    return 0;
}
