/**
 * D. Великая стена
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * У короля Людовика двое сыновей.
 * Они ненавидят друг друга, и король боится, что после его смерти страна будет уничтожена страшными войнами.
 * Поэтому Людовик решил разделить свою страну на две части, в каждой из которых будет властвовать один из его сыновей.
 * Он посадил их на трон в города A и B, и хочет построить минимально возможное количество фрагментов стены таким образом, чтобы не существовало пути из города A в город B.
 *
 * Страну, в которой властвует Людовик, можно упрощенно представить в виде прямоугольника m × n.
 * В некоторых клетках этого прямоугольника расположены горы, по остальным же можно свободно перемещаться
 * Кроме этого, ландшафт в некоторых клетках удобен для строительства стены, в остальных же строительство невозможно.
 * При поездках по стране можно перемещаться из клетки в соседнюю по стороне, только если ни одна из этих клеток не содержит горы или построенного фрагмента стены.
 *
 * Входные данные
 * В первой строке входного файла содержатся числа m и n (1 ≤ m, n ≤ 50).
 * Следующие m строк по n символов задают карту страны.
 * Символы обозначают: «#» — гора, «.» — место, пригодное для постройки стены, «-» — место, не пригодное для постройки стены, «A» и «B» — города A и B.
 *
 * Выходные данные
 * В первой строке выходного файла должно быть выведено минимальное количество фрагментов стены F, которые необходимо построить.
 * Далее нужно вывести карту в том же формате, как во входном файле. Клетки со стеной обозначьте символом «+».
 * Если невозможно произвести требуемую застройку, то выведите в выходной файл единственное число  - 1.
 */

#define DEBUG

#include <assert.h>
#include <string>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <set>
#include <vector>
#include <queue>

struct Coord
{
    std::size_t x, y;

    bool operator <(const Coord& a) const // for std::set
    {
        return x < a.x || (x == a.x && y < a.y);
    }
};

class Network
{
private:
    struct Edge
    {
        const std::size_t from_, to_;
        int flow_, throughput_;
        Edge* oppositeEdge = nullptr;

        Edge(std::size_t from, std::size_t to, int f, int c) : from_(from), to_(to), flow_(f), throughput_(c) {}
        Edge(std::size_t from, std::size_t to, int f, int c, Edge * back) : from_(from), to_(to), flow_(f), throughput_(c), oppositeEdge(back)
        {
        }
    };
    struct PathNode {
        bool origin_;
        Coord coord_;
        Edge* edge_;

        PathNode(bool origin, Edge* edge): origin_(origin), edge_(edge)
        {
        }

        PathNode(bool origin, Coord coord, Edge* edge): origin_(origin), coord_(coord), edge_(edge)
        {
        }
    };
    const std::size_t size_;
    const std::size_t sourceIndex_;
    const std::size_t sinkIndex_;
    std::vector<Edge*> edges_;
    std::vector<std::vector<PathNode>> paths_;
    int findMaxStream(std::size_t sourceIndex, std::size_t sinkIndex);
    void mark(std::vector<bool>& marks, std::size_t pathIndex);
public:
    Network(std::size_t size, std::size_t sourceIndex, std::size_t sinkIndex);
    ~Network();
    void addEdge(std::size_t cellIndex, std::size_t x, std::size_t y, int cv);
    void addOpposingEdge(std::size_t cellIndex, std::size_t opposingEdgeIndex);
    std::optional<std::set<Coord>> calcFieldCut();
};

int Network::findMaxStream(std::size_t sourceIndex, std::size_t sinkIndex)
{
    int maxThroughput = 0;
    while (true)
    {
        std::vector<int> d(paths_.size(), std::numeric_limits<int>::max());
        std::vector<std::pair<Edge*, int>> networkPath(paths_.size(), { nullptr, std::numeric_limits<int>::max() });
        std::priority_queue<std::pair<int, std::size_t>> queue;
        d[sourceIndex] = 0;
        queue.push({ 0, sourceIndex });
        bool end = false;
        while (!queue.empty() && !end)
        {
            std::size_t v = queue.top().second;
            int di = -queue.top().first;
            queue.pop();
            for (PathNode path : paths_[v])
            {
                std::size_t to = path.edge_->to_;
                if (path.edge_->flow_ < path.edge_->throughput_ && d[to] > di + 1)
                {
                    d[to] = di + 1;
                    networkPath[to] = { path.edge_, std::min(networkPath[v].second, path.edge_->throughput_ - path.edge_->flow_) };
                    if (to == sinkIndex)
                    {
                        end = true;
                        break;
                    }
                    queue.push({ -di - 1, to });
                }
            }
        }
        if (networkPath[sinkIndex].first == nullptr)
            break;
        std::size_t vertexIndex = sinkIndex;
        int throughput = networkPath[sinkIndex].second;
        if (throughput == std::numeric_limits<int>::max())
        {
            return std::numeric_limits<int>::max();
        }
        maxThroughput += throughput;
        while (networkPath[vertexIndex].first != nullptr)
        {
            networkPath[vertexIndex].first->flow_ += throughput;
            networkPath[vertexIndex].first->oppositeEdge->flow_ -= throughput;
            vertexIndex = networkPath[vertexIndex].first->from_;
        }
    }
    return maxThroughput;
}

void Network::mark(std::vector<bool> &marks, std::size_t pathIndex)
{
    marks[pathIndex] = true;
    for (PathNode path : paths_[pathIndex])
    {
        if (path.edge_->flow_ < path.edge_->throughput_ && !marks[path.edge_->to_])
        {
            mark(marks, path.edge_->to_);
        }
    }
}

Network::Network(std::size_t size, std::size_t sourceIndex, std::size_t sinkIndex):
    size_(size), sourceIndex_(sourceIndex), sinkIndex_(sinkIndex), paths_(size * 2)
{
}

Network::~Network()
{
    for (std::size_t i = 0; i < edges_.size(); ++i)
    {
        delete edges_[i];
    }
}

void Network::addEdge(std::size_t cellIndex, std::size_t x, std::size_t y, int cv)
{
    edges_.push_back(new Edge(cellIndex, cellIndex + size_, 0, cv));
    paths_[cellIndex].push_back(PathNode(true, {x, y}, edges_.back()));
    edges_.push_back(new Edge(cellIndex + size_, cellIndex, 0, 0, edges_.back()));
    paths_[cellIndex + size_].push_back(PathNode(false, edges_.back()));
    paths_[cellIndex].back().edge_->oppositeEdge = edges_.back();
}

void Network::addOpposingEdge(std::size_t cellIndex, std::size_t opposingEdgeIndex)
{
    edges_.push_back(new Edge(opposingEdgeIndex, cellIndex, 0, size_ + 1));
    paths_[opposingEdgeIndex].push_back(PathNode(false, edges_.back()));
    edges_.push_back(new Edge(cellIndex, opposingEdgeIndex, 0, 0, edges_.back()));
    paths_[cellIndex].push_back(PathNode(false, edges_.back()));
    paths_[opposingEdgeIndex].back().edge_->oppositeEdge = edges_.back();
}

std::optional<std::set<Coord>> Network::calcFieldCut()
{
    int max_stream = findMaxStream(sourceIndex_, sinkIndex_);

    if (max_stream > size_)
    {
        return std::nullopt;
    }

    std::vector <bool> marks(2 * size_);
    mark(marks, sourceIndex_);

    std::set<Coord> cut;
    for (std::size_t i = 0; i < 2 * size_; i++)
    {
        if (!marks[i])
        {
            continue;
        }
        for (PathNode path : paths_[i])
        {
            if (!path.origin_ || marks[path.edge_->to_])
            {
                continue;
            }
            cut.insert(path.coord_);
        }
    }

    return std::make_optional(cut);
}

int main() {
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    std::cout.tie(nullptr);
#endif
    std::size_t rowCount, colCount;
    cin >> rowCount >> colCount;
    std::size_t n = rowCount * colCount;
    std::vector <std::string> field(rowCount);
    for (std::size_t i = 0; i < rowCount; ++i)
    {
        cin >> field[i];
    }

    std::size_t sourceIndex = n, sinkIndex = n;
    for (std::size_t i = 0, cellIndex = 0; i < rowCount; ++i)
    {
        for (std::size_t j = 0; j < colCount; ++j, ++cellIndex)
        {

            if (field[i][j] == 'A')
            {
                sourceIndex = cellIndex;
            }
            else if (field[i][j] == 'B')
            {
                sinkIndex = cellIndex;
            }
        }
    }
    assert(sourceIndex < n);
    assert(sinkIndex < n);

    Network network(n, sourceIndex, sinkIndex);

    const std::vector<std::pair<int, int>> shifts = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };
    for (std::size_t i = 0, cellIndex = 0, oppositeEdge = n; i < rowCount; ++i)
    {
        for (std::size_t j = 0; j < colCount; ++j, ++cellIndex, ++oppositeEdge)
        {

            if (field[i][j] == '#')
            {
                continue;
            }

            int cv = field[i][j] == '.' ? 1 : static_cast<int>(n + 1);

            network.addEdge(cellIndex, i, j, cv);
            std::vector<std::size_t> adjastentCells = { cellIndex - colCount, cellIndex + 1, cellIndex + colCount, cellIndex - 1 };
            for (std::size_t k = 0; k < 4; k++)
            {
                std::size_t adjastentCellIndex = adjastentCells[k];
                std::size_t ip = i + shifts[k].first, jp = j + shifts[k].second;
                if (adjastentCellIndex >= n || (k == 1 && j == colCount - 1) || (k == 3 && j == 0) || field[ip][jp] == '#')
                {
                    continue;
                }
                network.addOpposingEdge(cellIndex, adjastentCellIndex + n);
            }
        }
    }

    auto cut = network.calcFieldCut();
    if (!cut)
    {
        std::cout << "-1\n";
        return 0;
    }
    for (Coord c : cut.value())
    {
        field[c.x][c.y] = '+';
    }
    std::cout << cut.value().size() << "\n";
    for (std::string s : field)
    {
        std::cout << s << "\n";
    }

    return 0;
}

