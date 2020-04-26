/**
 * B. Разрез
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Найдите минимальный разрез между вершинами 1 и n в заданном неориентированном графе.
 *
 * Входные данные
 * На первой строке входного файла содержится n (2≤n≤100) — число вершин в графе и m (0≤m≤400) — количество ребер.
 * На следующих m строках входного файла содержится описание ребер.
 * Ребро описывается номерами вершин, которые оно соединяет, и его пропускной способностью (положительное целое число, не превосходящее 10000000),
 * при этом никакие две вершины не соединяются более чем одним ребром.
 *
 * Выходные данные
 * На первой строке выходного файла должны содержаться количество ребер в минимальном разрезе и их суммарная пропускная способность.
 * На следующей строке выведите возрастающую последовательность номеров ребер (ребра нумеруются в том порядке, в каком они были заданы во входном файле).
 */

#define DEBUG

#include <algorithm>
#include <assert.h>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <limits>
#include <queue>
#include <vector>

class Network
{
private:
    struct Edge {
        const std::size_t index_;
        const std::size_t opposisiteIndex_;
        const std::size_t end_;
        const int cost_;
        std::size_t reachedFrom_;
        int usedCost_;

        Edge(std::size_t index, std::size_t opposisiteIndex, std::size_t end, int cost)
            : index_(index), opposisiteIndex_(opposisiteIndex), end_(end), cost_(cost), usedCost_(0)
        {
        }
    };
    std::vector<Edge> edges_;
    std::vector<std::vector<std::size_t>> vertices_;
    int totalThroughput_;
    std::vector<std::size_t> minCut_;
    std::size_t sourceIndex_;
    std::size_t sinkIndex_;

    void unwindPath(const std::size_t edgeIndex)
    {
        const std::size_t size = edges_.size();

        int throughput = std::numeric_limits<int>::max();
        std::size_t nextEdgeIndex = edgeIndex;
        while(nextEdgeIndex != size)
        {
            const Edge& edge = edges_[nextEdgeIndex];
            const int edgeThroughput = edge.cost_ - edge.usedCost_;
            assert(edgeThroughput > 0);
            if (throughput > edgeThroughput)
            {
                throughput = edgeThroughput;
            }
            nextEdgeIndex = edge.reachedFrom_;
        }
        assert(throughput > 0 && throughput < std::numeric_limits<int>::max());

        nextEdgeIndex = edgeIndex;
        while(nextEdgeIndex != size)
        {
            Edge& edge = edges_[nextEdgeIndex];
            Edge& oppositeEdge = edges_[edge.opposisiteIndex_];
            edge.usedCost_ += throughput;
            oppositeEdge.usedCost_ -= throughput;
            assert(edge.cost_ - edge.usedCost_ >= 0);
            nextEdgeIndex = edge.reachedFrom_;
        }
        totalThroughput_ += throughput;
    }
    void findMinCut(const std::vector<bool>& visited)
    {
        for (std::size_t i = 0; i < visited.size(); ++i)
        {
            if (!visited[i])
            {
                for (std::size_t edgeIndex : vertices_[i])
                {
                    const Edge& edge = edges_[edgeIndex];
                    if (visited[edge.end_])
                    {
                        minCut_.push_back(edgeIndex);
                    }
                }
            }
        }
        std::sort(minCut_.begin(), minCut_.end());
    }
    bool removeShortestPath()
    {
        const std::size_t size = edges_.size();
        std::vector<bool> visited(vertices_.size());
        std::queue<std::size_t> queue;
        for (std::size_t edgeIndex : vertices_[sourceIndex_])
        {
            Edge& edge = edges_[edgeIndex];
            if (edge.cost_ - edge.usedCost_ > 0)
            {
                edge.reachedFrom_ = size;
                queue.push(edgeIndex);
            }
        }
        visited[sourceIndex_] = true;

        while (!queue.empty())
        {
            const std::size_t edgeIndex = queue.front();
            queue.pop();
            Edge& edge = edges_[edgeIndex];
            assert(edge.index_ == edgeIndex);
            if (visited[edge.end_])
            {
                continue;
            }
            visited[edge.end_] = true;
            if (edge.end_ == sinkIndex_)
            {
                unwindPath(edgeIndex);
                return true;
            }
            for (std::size_t edgeIndex2 : vertices_[edge.end_])
            {
                Edge& edge2 = edges_[edgeIndex2];
                if (edge2.cost_ - edge2.usedCost_ > 0 && !visited[edge2.end_])
                {
                    edge2.reachedFrom_ = edgeIndex;
                    queue.push(edgeIndex2);
                }
            }
        }

        findMinCut(visited);
        return false;
    }
public:
    Network(std::size_t size, std::size_t sourceIndex, std::size_t sinkIndex): vertices_(size), sourceIndex_(sourceIndex), sinkIndex_(sinkIndex)
    {
    }
    void addEdge(std::size_t start, std::size_t end, int cost)
    {
        assert(start < vertices_.size());
        assert(end < vertices_.size());
        const std::size_t index1 = edges_.size();
        const std::size_t index2 = edges_.size() + 1;
        edges_.emplace_back(index1, index2, end, cost);
        edges_.emplace_back(index2, index1, start, cost);
        vertices_[start].push_back(index1);
        vertices_[end].push_back(index2);
    }
    void calcMinCut()
    {
        totalThroughput_ = 0;
        for (Edge& edge : edges_)
        {
            edge.usedCost_ = 0;
        }
        minCut_.resize(0);

        while (removeShortestPath())
            ;
    }
    void getMinCut(std::ostream& os)
    {
        os << minCut_.size() << ' ' << totalThroughput_ << '\n';
        for (std::size_t edgeIndex : minCut_)
        {
            os << (edgeIndex / 2 + 1) << ' ';
        }
        os << '\n';
    }
};

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

    std::size_t vertexCount, edgeCount;
    cin >> vertexCount >> edgeCount;

    Network network(vertexCount, 0, vertexCount - 1);

    for (std::size_t i = 0; i < edgeCount; ++i)
    {
        std::size_t start, end;
        int cost;
        cin >> start >> end >> cost;

        network.addEdge(start - 1, end - 1, cost);
    }

    network.calcMinCut();
    network.printOut(std::cout);

    return 0;
}

