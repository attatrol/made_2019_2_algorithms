/**
  * C. План эвакуации
  *
  * ввод стандартный ввод
  * вывод стандартный вывод
  *
  * В городе есть муниципальные здания и бомбоубежища, которые были специально построены для эвакуации служащих в случае ядерной войны.
  * Каждое бомбоубежище имеет ограниченную вместительность по количеству людей, которые могут в нем находиться.
  * В идеале все работники из одного муниципального здания должны были бы бежать к ближайшему бомбоубежищу.
  * Однако, в таком случае, некоторые бомбоубежища могли бы переполниться, в то время как остальные остались бы наполовину пустыми.
  * Чтобы разрешить эту проблему Городской Совет разработал специальный план эвакуации.
  * Вместо того, чтобы каждому служащему индивидуально приписать, в какое бомбоубежище он должен бежать, для каждого муниципального здания определили,
  * сколько служащих из него в какое бомбоубежище должны бежать.
  * Задача индивидуального распределения была переложена на внутреннее управление муниципальных зданий.
  * План эвакуации учитывает количество служащих в каждом здании — каждый служащий должен быть учтен в плане и в каждое бомбоубежище может быть направлено количество служащих,
  * не превосходящее вместимости бомбоубежища.
  * Городской Совет заявляет, что их план эвакуации оптимален в том смысле, что суммарное время эвакуации всех служащих города минимально.
  * Мэр города, находящийся в постоянной конфронтации с Городским Советом, не слишком то верит этому заявлению.
  * Поэтому он нанял Вас в качестве независимого эксперта для проверки плана эвакуации.
  * Ваша задача состоит в том, чтобы либо убедиться в оптимальности плана Городского Совета, либо доказать обратное,
  * представив в качестве доказательства другой план эвакуации с меньшим суммарным временем для эвакуации всех служащих.
  * Карта города может быть представлена в виде квадратной сетки.
  * Расположение муниципальных зданий и бомбоубежищ задается парой целых чисел,
  * а время эвакуации из муниципального здания с координатами (X_i, Y_i) в бомбоубежище с координатами (P_j, Q_j) составляет D_{ij} = |X_i - P_j|+|Y_i - Q_j|+1 минут.
  *
  * Входные данные
  * Входной файл содержит описание карты города и плана эвакуации, предложенного Городским Советом.
  * Первая строка входного файла содержит два целых числа N (1 ≤ N ≤ 100) и M (1 ≤ M ≤ 100), разделенных пробелом.
  * N — число муниципальных зданий в городе (все они занумерованы числами от 1 до N), M — число бомбоубежищ (все они занумерованы числами от 1 до M).
  * Последующие N строк содержат описания муниципальных зданий.
  * Каждая строка содержит целые числа X_i, Y_i и B_i, разделенные пробелами, где X_i, Y_i ( - 1000 ≤ X_i, Y_i ≤ 1000) — координаты здания,
  * а B i (1 ≤ B i ≤ 1000) — число служащих в здании.
  * Описание бомбоубежищ содержится в последующих M строках.
  * Каждая строка содержит целые числа P_j, Q_j и C_j, разделенные пробелами, где P_j, Q_j ( - 1000 ≤ P_j, Q_j ≤ 1000) — координаты бомбоубежища,
  * а C_j (1 ≤ C_j ≤ 1000) — вместимость бомбоубежища.
  * В последующих N строках содержится описание плана эвакуации.
  * Каждая строка представляет собой описание плана эвакуации для отдельного здания.
  * План эвакуации из i-го здания состоит из M целых чисел E_{ij}, разделенных пробелами.
  * E_{ij} (0 ≤ E_{ij} ≤ 10000) — количество служащих, которые должны эвакуироваться из i-го здания в j-е бомбоубежище.
  * Гарантируется, что план, заданный во входном файле, корректен.
  *
  * Выходные данные
  * Если план эвакуации Городского Совета оптимален, то выведите одно слово OPTIMAL.
  * В противном случае выведите на первой строке слово SUBOPTIMAL, а в последующих N строках выведите Ваш план эвакуации (более оптимальный) в том же формате, что и во входном файле.
  * Ваш план не обязан быть оптимальным, но должен быть лучше плана Городского Совета.
  *
  * Примеры
  *
  * Входные данные
  * 3 4
  * -3 3 5
  * -2 -2 6
  * 2 2 5
  * -1 1 3
  * 1 1 4
  * -2 -2 7
  * 0 -1 3
  * 3 1 1 0
  * 0 0 6 0
  * 0 3 0 2
  *
  * Выходные данные
  * SUBOPTIMAL
  * 3 0 1 1
  * 0 0 6 0
  * 0 4 0 1
  *
  * Входные данные
  * 3 4
  * -3 3 5
  * -2 -2 6
  * 2 2 5
  * -1 1 3
  * 1 1 4
  * -2 -2 7
  * 0 -1 3
  * 3 0 1 1
  * 0 0 6 0
  * 0 4 0 1
  *
  * Выходные данные
  * OPTIMAL
  */

#define DEBUG

#include <algorithm>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>

void minimalChange(const std::size_t i, const std::size_t j, const std::size_t reverseEdgeOffset,
                   std::vector<std::vector<int>>& edges, const std::vector<std::vector<std::size_t>>& parent)
{
    std::size_t k = parent[i][j];
    if (k == i)
    {
        if (i < reverseEdgeOffset)
        {
            edges[j - reverseEdgeOffset][i]--;
        }
        else
        {
            edges[i - reverseEdgeOffset][j]++;
        }
    }
    else
    {
        minimalChange(i, k, reverseEdgeOffset, edges, parent);
        minimalChange(k, j, reverseEdgeOffset, edges, parent);
    }
}

struct Place
{
    int x_;
    int y_;
    int size_;
};

static const int INFINITE_DISTANCE = 1000 * 2 * 1000 + 1;

int main() {
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    std::cout.tie(nullptr);
#endif
    std::size_t buildingCount, shelterCount;
    cin >> buildingCount >> shelterCount;

    std::vector<Place> buildings(buildingCount);
    for(std::size_t i = 0; i < buildingCount; ++i)
    {
        Place& building = buildings[i];
        cin >> building.x_ >> building.y_ >> building.size_;
    }
    std::vector<Place> shelters(shelterCount);
    for(std::size_t i = 0; i < shelterCount; ++i)
    {
        Place& shelter = shelters[i];
        cin >> shelter.x_ >> shelter.y_ >> shelter.size_;
    }

    std::vector<std::vector<int>> edges(buildingCount);
    for(std::size_t i = 0; i < buildingCount; ++i)
    {
        edges[i].resize(shelterCount);
        for(std::size_t j = 0; j < shelterCount; ++j)
        {
            cin >> edges[i][j];
            shelters[j].size_ -= edges[i][j];
        }
    }

    std::size_t vertexCount = buildingCount + shelterCount;
    std::vector<std::vector<int>> distances(vertexCount);
    std::vector<std::vector<std::size_t>> parent(vertexCount);
    for(std::size_t i = 0; i < vertexCount; ++i)
    {
        distances[i].resize(vertexCount);
        std::fill(distances[i].begin(), distances[i].end(), INFINITE_DISTANCE);
        parent[i].resize(vertexCount);
        std::fill(parent[i].begin(), parent[i].end(), i);
    }
    for(std::size_t i = 0; i < shelterCount; ++i)
    {
        for(std::size_t j = 0; j < buildingCount; ++j)
        {
            int distance = std::abs(buildings[j].x_ - shelters[i].x_) + std::abs(buildings[j].y_ - shelters[i].y_) + 1;
            if (edges[j][i] > 0)
            {
                distances[i][j + shelterCount] = -distance;
            }
            distances[j + shelterCount][i] = distance;
        }
    }

    for(std::size_t i = 0; i < vertexCount; ++i)
    {
        for(std::size_t j = 0; j < vertexCount; ++j)
        {
            for(std::size_t k = 0; k < vertexCount; ++k)
            {
                if (distances[j][k] > distances[j][i] + distances[i][k])
                {
                    distances[j][k] = distances[j][i] + distances[i][k];
                    parent[j][k] = i;
                    if (distances[j][k] < 0 && (j == k || (j < shelterCount && k < shelterCount && shelters[k].size_ > 0)))
                    {
                        std::cout << "SUBOPTIMAL\n";
                        minimalChange(j, k, shelterCount, edges, parent);
                        for(std::size_t l = 0; l < buildingCount; ++l)
                        {
                            for(std::size_t m = 0; m < shelterCount; ++m)
                            {
                                std::cout << edges[l][m] << ' ';
                            }
                            std::cout << '\n';
                        }
                        return 0;
                    }
                }
            }
        }
    }
    std::cout << "OPTIMAL\n";

    return 0;
}
