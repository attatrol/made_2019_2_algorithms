/**
 * A. Просто поток
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Дана система из узлов и труб, по которым может течь вода.
 * Для каждой трубы известна наибольшая скорость, с которой вода может протекать через нее.
 * Известно, что вода течет по трубам таким образом, что за единицу времени в каждый узел (за исключением двух — источника и стока) втекает ровно столько воды, сколько из него вытекает.
 * Ваша задача — найти наибольшее количество воды, которое за единицу времени может протекать между источником и стоком, а также скорость течения воды по каждой из труб.
 * Трубы являются двусторонними, то есть вода в них может течь в любом направлении.
 * Между любой парой узлов может быть более одной трубы.
 *
 * Входные данные
 * В первой строке записано натуральное число N — количество узлов в системе (2≤N≤100).
 * Известно, что источник имеет номер 1, а сток номер N.
 * Во второй строке записано натуральное M (1≤M≤5000) — количество труб в системе.
 * Далее в M строках идет описание труб.
 * Каждая труба задается тройкой целых чисел Ai, Bi, Ci, где Ai, Bi — номера узлов, которые соединяет данная труба (Ai≠Bi),
 * а Ci (0≤Ci≤10^4) — наибольшая допустимая скорость течения воды через данную трубу.
 *
 * Выходные данные
 * В первой строке выведите наибольшее количество воды, которое протекает между источником и стоком за единицу времени.
 * Далее выведите M строк, в каждой из которых выведите скорость течения воды по соответствующей трубе.
 * Если направление не совпадает с порядком узлов, заданным во входных данных, то выводите скорость со знаком минус.
 * Числа выводите с точностью 10^−3.
 */

#define DEBUG

#include <algorithm>
#include <assert.h>
#include <cmath>
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
    int totalCost_;

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
        totalCost_ += throughput;
    }
    bool removeShortestPath()
    {
        const std::size_t size = edges_.size();
        const std::size_t sinkIndex = vertices_.size() - 1;
        std::vector<bool> visited(vertices_.size());
        std::queue<std::size_t> queue;
        for (std::size_t edgeIndex : vertices_[0])
        {
            Edge& edge = edges_[edgeIndex];
            if (edge.cost_ - edge.usedCost_ > 0)
            {
                edge.reachedFrom_ = size;
                queue.push(edgeIndex);
            }
        }
        visited[0] = true;

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
            if (edge.end_ == sinkIndex)
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
        return false;
    }
public:
    Network(std::size_t size): vertices_(size)
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
    int calcTotalCost()
    {
        totalCost_ = 0;
        for (Edge& edge : edges_)
        {
            edge.usedCost_ = 0;
        }
        while (removeShortestPath())
            ;
        return totalCost_;
    }
    void printOut(std::ostream& os)
    {
        for (std::size_t i = 0; i < edges_.size(); i += 2)
        {
            os << edges_[i].usedCost_ << '\n';
        }
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

    Network network(vertexCount);

    for (std::size_t i = 0; i < edgeCount; ++i)
    {
        std::size_t start, end;
        int cost;
        cin >> start >> end >> cost;

        network.addEdge(start - 1, end - 1, cost);
    }

    std::cout << network.calcTotalCost() << '\n';
    network.printOut(std::cout);

    return 0;
}

