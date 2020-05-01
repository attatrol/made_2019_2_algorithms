/**
  * A. Максимальный поток минимальной стоимости
  *
  * ввод стандартный ввод
  * вывод стандартный вывод
  *
  * Задан ориентированный граф, каждое ребро которого обладает пропускной способностью и стоимостью.
  * Найдите максимальный поток минимальной стоимости из вершины с номером 1 в вершину с номером n.
  *
  * Входные данные
  * Первая строка входного файла содержит n и m — количество вершин и количество ребер графа (2≤n≤100, 0≤m≤1000).
  * Следующие m строк содержат по четыре целых числа числа: номера вершин, которые соединяет соответствующее ребро графа, его пропускную способность и его стоимость.
  * Пропускные способности и стоимости не превосходят 10^5.
  *
  * Выходные данные
  * В выходной файл выведите одно число — цену максимального потока минимальной стоимости из вершины с номером 1 в вершину с номером n.
  * Ответ не превышает 2^63−1.
  * Гарантируется, что в графе нет циклов отрицательной стоимости.
  */

//#define DEBUG

#include <algorithm>
#include <assert.h>
#include <string>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>
#include <queue>

class Network
{
private:
    struct Edge
    {
        const std::size_t from_, to_;
        int flow_;
        const int throughput_, price_;
        const std::size_t oppositeEdgeIndex_;
        const bool directEdge_;

        Edge(bool directEdge, std::size_t from, std::size_t to, int throughput, int price, std::size_t oppositeEdgeIndex) :
            from_(from), to_(to), flow_(0), throughput_(throughput), price_(price), oppositeEdgeIndex_(oppositeEdgeIndex), directEdge_(directEdge)
        {
        }
    };

    const std::size_t vertexCount_;
    const std::size_t sourceIndex_;
    const std::size_t sinkIndex_;
    std::vector<Edge> edges_;
    std::vector<std::vector<std::size_t>> vertices_;
public:
    Network(std::size_t vertexCount, std::size_t sourceIndex, std::size_t sinkIndex) : vertexCount_(vertexCount), sourceIndex_(sourceIndex), sinkIndex_(sinkIndex), vertices_(vertexCount)
    {
        assert(sourceIndex != sinkIndex);
        assert(sourceIndex < vertexCount);
        assert(sinkIndex < vertexCount);
    }
    void addEdge(std::size_t from, std::size_t to, int throughput, int price)
    {
        const std::size_t edgeCount = edges_.size();
        edges_.emplace_back(true, from, to, throughput, price, edgeCount + 1);
        edges_.emplace_back(false, to, from, 0, -price, edgeCount);
        vertices_[from].push_back(edgeCount);
        vertices_[to].push_back(edgeCount + 1);
    }
    std::vector<long long> runBellmanFord()
    {
        std::vector<long long> pathCosts(vertexCount_, std::numeric_limits<long long>::max());
        pathCosts[sourceIndex_] = 0;
        for (std::size_t i = 0; i < vertexCount_; ++i)
        {
            for (std::size_t j = 0, edgeCount = edges_.size(); j < edgeCount; ++j)
            {
                const Edge& edge = edges_[j];
                if (!edge.directEdge_)
                {
                    continue;
                }
                if (pathCosts[edge.to_] > pathCosts[edge.from_] + edge.price_)
                {
                    pathCosts[edge.to_] = pathCosts[edge.from_] + edge.price_;
                }
            }
        }
        return pathCosts;
    }
    void runDijkstra(std::vector<long long>& phi, std::vector<long long>& distances, std::vector<std::size_t>& parents)
    {
        std::fill(distances.begin(), distances.end(), std::numeric_limits<long long>::max());
        distances[sourceIndex_] = 0;
        std::priority_queue<std::pair<long long, std::size_t>> q;
        for (std::size_t i = 0; i < vertexCount_; ++i)
        {
            q.push(std::make_pair(i == sourceIndex_? 0 : -std::numeric_limits<long long>::max(), i));
        }

        while (!q.empty()) {
            std::size_t vertex = q.top().second;
            long long cur_dist = -q.top().first;
            q.pop();
            if (cur_dist > distances[vertex])
            {
                continue;
            }
            if (distances[vertex] == std::numeric_limits<long long>::max())
            {
                break;
            }
            for (std::size_t i = 0, vertexEdgesCount = vertices_[vertex].size(); i < vertexEdgesCount; i++)
            {
                std::size_t ind = vertices_[vertex][i];
                if (edges_[ind].flow_ == edges_[ind].throughput_)
                {
                    continue;
                }
                std::size_t to = edges_[ind].to_;
                long long w = edges_[ind].price_ + phi[vertex] - phi[to];
                if (cur_dist + w < distances[to])
                {
                    distances[to] = cur_dist + w;
                    parents[to] = ind;
                    q.push(std::make_pair(-distances[to], to));
                }
            }
        }
    }
    long long calcMinCostMaxFlow(int flow)
    {
        long long result = 0;

        std::vector<long long> phi = runBellmanFord();
        std::vector<long long>distances(vertexCount_);
        std::vector<std::size_t> parents(vertexCount_);

        while (true) {

            runDijkstra(phi, distances, parents);

            if (distances[sinkIndex_] == std::numeric_limits<long long>::max())
            {
                return result;
            }

            for (std::size_t i = 0; i < vertexCount_; i++)
            {
                phi[i] = phi[i] + distances[i];
            }

            int flowSize = flow;
            std::size_t cur = sinkIndex_;
            while (cur != sourceIndex_) {
                const Edge& edge = edges_[parents[cur]];
                std::size_t from = edge.from_;
                int localFlowSize = edge.throughput_ - edge.flow_;
                flowSize = std::min(flowSize, localFlowSize);
                cur = from;
            }

            flow -= flowSize;
            cur = sinkIndex_;
            while (cur != sourceIndex_) {
                Edge& tmp = edges_[parents[cur]];
                std::size_t from = tmp.from_;
                tmp.flow_ += flowSize;
                edges_[tmp.oppositeEdgeIndex_].flow_ -= flowSize;
                result += 1ll * flowSize * tmp.price_;
                cur = from;
            }

            if (flow == 0)
            {
                break;
            }
        }
        return result;
    }
};


int main() {
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
        std::size_t from, to;
        int throughput, price;
        cin >> from >>to >> throughput >> price;
        network.addEdge(from - 1, to - 1, throughput, price);
    }

    std::cout << network.calcMinCostMaxFlow(std::numeric_limits<int>::max());

    return 0;
}

