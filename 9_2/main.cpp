/**
  * B. Задача о назначениях
  *
  * ввод стандартный ввод
  * вывод стандартный вывод
  *
  * Дана целочисленная матрица C размера n×n.
  * Требуется выбрать n ячеек так, чтобы в каждой строке и каждом столбце была выбрана ровно одна ячейка, а сумма значений в выбранных ячейках была минимальна.
  *
  * Входные данные
  * Первая строка входного файла содержит n (2≤n≤300).
  * Каждая из последующих n строк содержит по n чисел: C_{ij}.
  * Все значения во входном файле неотрицательны и не превосходят 10^6.
  *
  * Выходные данные
  * В первую строку выходного файла выведите одно число — искомая минимизируемая величина.
  * Далее выведите n строк по два числа в каждой — номер строки и столбца клетки, участвующей в оптимальном назначении.
  * Пары чисел можно выводить в произвольном порядке.
  */

#define DEBUG

#include <algorithm>
#include <assert.h>
#include <string>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <set>
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
    std::vector<std::pair<std::size_t, std::size_t>> getPermutation()
    {
        std::vector<std::pair<std::size_t, std::size_t>> result;
        for (std::size_t i = 0; i < edges_.size(); ++i)
        {
            const Edge& edge = edges_[i];
            if (edge.from_ != sourceIndex_ && edge.to_ != sinkIndex_ && edge.directEdge_ && edge.flow_)
            {
                result.emplace_back(edge.from_, edge.to_);
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
    std::size_t n;
    cin >> n;
    const std::size_t vertexCount = 2 * n + 2;
    const std::size_t sourceIndex = 0;
    const std::size_t sinkIndex = 2 * n + 1;
    Network network(vertexCount, sourceIndex, sinkIndex);

    for (std::size_t i = 1; i <= n; ++i)
    {
        network.addEdge(sourceIndex, i, 1, 0);
    }
    for (std::size_t i = 1; i <= n; ++i)
    {
        for (std::size_t j = n + 1; j <= 2 *n; ++j)
        {
            int price;
            cin >> price;
            network.addEdge(i, j, static_cast<int>(vertexCount + 1), price);
        }
    }
    for (std::size_t i = n + 1; i <= 2 * n; ++i)
    {
        network.addEdge(i, sinkIndex, 1, 0);
    }

    std::cout << network.calcMinCostMaxFlow(std::numeric_limits<int>::max()) << '\n';

    for (auto& permute : network.getPermutation())
    {
        std::cout << permute.first << ' ' << permute.second - n << '\n';
    }

    return 0;
}

