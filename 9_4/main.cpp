/**
 * D. Автоматное программирование
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * В один замечательный день в компанию «X» завезли k автоматов.
 * И не простых автоматов, а автоматов-программистов!
 * Это был последний неудачный шаг перед переходом на андроидов-программистов, но это уже совсем другая история.
 *
 * В компании сейчас n задач, для каждой из которых известно время начала ее выполнения s_i, длительность ее выполнения t_i и прибыль компании от ее завершения c_i.
 * Любой автомат может выполнять любую задачу, ровно одну в один момент времени.
 * Если автомат начал выполнять задачу, то он занят все моменты времени с s_i по s_i+t_i−1 включительно и не может переключиться на другую задачу.
 * Вам требуется выбрать набор задач, которые можно выполнить с помощью этих k автоматов и который принесет максимальную суммарную прибыль.
 *
 * Входные данные
 * В первой строке записаны два целых числа n и k (1≤n≤1000, 1≤k≤50) — количество задач и количество автоматов, соответственно.
 * В следующих n строках через пробелы записаны тройки целых чисел s_i,t_i,c_i (1≤s_i,t_i≤109, 1≤c_i≤106),
 * s_i — время начала выполнения i-го задания, t_i — длительность i-го задания, а c_i — прибыль от его выполнения.
 *
 * Выходные данные
 * Выведите n целых чисел x1,x2,…,xn. Число xi должно быть равно 1, если задачу i следует выполнить, и 0 в противном случае.
 * Если оптимальных решений несколько, то выведите любое из них.
 */

#define DEBUG

#include <algorithm>
#include <assert.h>
#include <string>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>
#include <queue>
#include <unordered_map>

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
        const bool taskEdge_;
        const std::size_t taskIndex_;

        Edge(bool directEdge, std::size_t from, std::size_t to, int throughput, int price, std::size_t oppositeEdgeIndex, bool taskEdge, std::size_t taskIndex) :
            from_(from), to_(to), flow_(0), throughput_(throughput), price_(price),
            oppositeEdgeIndex_(oppositeEdgeIndex), directEdge_(directEdge), taskEdge_(taskEdge), taskIndex_(taskIndex)
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
    void addIdleEdge(std::size_t from, std::size_t to, int throughput)
    {
        const std::size_t edgeCount = edges_.size();
        edges_.emplace_back(true,  from, to,   throughput, 0, edgeCount + 1, false, 0);
        edges_.emplace_back(false, to,   from, 0,         -0, edgeCount,     false, 0);
        vertices_[from].push_back(edgeCount);
        vertices_[to].push_back(edgeCount + 1);
    }
    void addTaskEdge(std::size_t from, std::size_t to, int price, std::size_t taskIndex)
    {
        const std::size_t edgeCount = edges_.size();
        edges_.emplace_back(true,  from, to,   1, -price, edgeCount + 1, true, taskIndex);
        edges_.emplace_back(false, to,   from, 0, price, edgeCount,     true, taskIndex);
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
    std::vector<bool> getExecutedTask(std::size_t taskCount)
    {
        std::vector<bool> result(taskCount);
        for (const Edge& edge : edges_)
        {
            if (edge.taskEdge_ && edge.directEdge_ && edge.flow_)
            {
                result[edge.taskIndex_] = true;
            }
        }
        return result;
    }
};

struct Task
{
    std::size_t index_;
    int start_;
    int end_;
    int cost_;
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

    std::size_t taskCount;
    int flowSize;
    cin >> taskCount >> flowSize;

    std::vector<Task> tasks(taskCount);
    std::vector<int> timeMoments(2 * taskCount);

    for (std::size_t i = 0; i < taskCount; ++i)
    {
        Task& task = tasks[i];
        int start, duration, cost;
        cin >> start >> duration >> cost;
        const int end = start + duration;
        task.index_ = i;
        task.start_ = start;
        task.end_ = end;
        task.cost_ = cost;
        timeMoments[2 * i] = start;
        timeMoments[2 * i + 1] = end;
    }
    std::sort(timeMoments.begin(), timeMoments.end());
    std::unordered_map<int, std::size_t> timeIndexes;
    std::size_t timeIndex = 0;
    for (int timeMoment : timeMoments)
    {
        if (timeIndexes.find(timeMoment) == timeIndexes.end())
        {
            timeIndexes.emplace(timeMoment, timeIndex++);
        }
    }
    const std::size_t timeMomentCount = timeIndexes.size();
    assert(timeMomentCount > 1);

    Network network(timeMomentCount, 0, timeMomentCount - 1);
    for (std::size_t i = 0; i < timeMomentCount - 1; ++i)
    {
        network.addIdleEdge(i, i + 1, flowSize);
    }
    for (std::size_t i = 0; i < taskCount; ++i)
    {
        const Task& task = tasks[i];
        const std::size_t startIndex = timeIndexes[task.start_];
        const std::size_t endIndex = timeIndexes[task.end_];
        assert(startIndex <= endIndex);
        network.addTaskEdge(startIndex, endIndex, task.cost_, task.index_);
    }

    network.calcMinCostMaxFlow(flowSize);

    auto executedTasks = network.getExecutedTask(taskCount);

    for (bool task : executedTasks)
    {
        std::cout << (task ? "1 " : "0 ");
    }

    return 0;
}
