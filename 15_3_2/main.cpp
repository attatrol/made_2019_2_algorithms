#define DEBUG

#include <assert.h>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>

using namespace std;

const int N = 1e6 + 5, M = 2e6 + 6;
int n, arr[N];

int root[N], L[M], R[M], seg[M];
int nodes;

int build(int i, int j)
{
    int at = ++nodes;

    seg[at] = 0;

    if(i != j)
    {
        int mid = (i+j)>>1;
        L[at] = build(i, mid);
        R[at] = build(mid+1, j);
    }

    return at;
}

int updateNode(int nd, int i, int j, int p)
{
    int at = ++nodes;

    if(i == j)
    {
        seg[at] = seg[nd] + 1;
    } else
    {
        int mid = (i+j)>>1;
        if(p <= mid)
        {
            L[at] = updateNode(L[nd], i, mid, p);
            R[at] = R[nd];
        } else
        {
            L[at] = L[nd];
            R[at] = updateNode(R[nd], mid+1, j, p);
        }

        seg[at] = seg[L[at]] + seg[R[at]];
    }

    return at;
}

int query(int l, int r, int i, int j, int k)
{
    if(i == j)
    {
        return i;
    } else
    {
        int cnt_l = seg[L[r]] - seg[L[l]];
        int mid = (i+j)>>1;

        if(k <= cnt_l)
        {
            return query(L[l], L[r], i, mid, k);
        }
        return query(R[l], R[r], mid+1, j, k-cnt_l);
    }
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

    std::size_t arraySize;
    long long a, l, m;
    cin >> arraySize >> a >> l >> m;
    n = arraySize;
    vector<int> v;
    for (std::size_t i = 0; i < arraySize; ++i)
    {
        arr[i] = a;
        v.pb(a);
        a = (a * l + m) % 1000000000ll;
    }

#ifdef DEBUG
    for (std::size_t i = 0; i < arraySize; ++i)
    {
        std::cout << arr[i] <<  ' ';
    }
    std::cout << '\n';
#endif

    sort(v.begin(), v.end());
    v.resize(unique(v.begin(), v.end()) - v.begin());
    map<int, int> mp;
    for(int i = 0; i < (int)v.size(); i++)
        mp[v[i]] = i;
    for(int i = 0; i < n; i++)
        arr[i] = mp[arr[i]];

    root[0] = build(0, n-1);
    for(int i = 0; i < n; i++)
        root[i+1] = updateNode(root[i], 0, n-1, arr[i]);

    std::size_t queryCount;
    cin >> queryCount;

    long long querySum = 0;
    for (std::size_t i = 0; i < queryCount; ++i)
    {
        std::size_t queryCount1, x, lx, mx, y, ly, my, k, lk, mk;
        cin >> queryCount1 >> x >> lx >> mx >> y >> ly >> my >> k >> lk >> mk;
        std::size_t l = std::min(x, y);
        std::size_t r = std::max(x, y);
        for (std::size_t j = 0; j < queryCount1; ++j)
        {
            int mid = query(root[l - 1], root[r], 0, n - 1, k);
#ifdef DEBUG
            std::cout << "query l = " << l << " r = " << r << " kStat = " << k << " value = " << mid << '\n';
#endif
            querySum += v[mid];
            x = ((l - 1) * lx + mx) % arraySize + 1;
            y = ((r - 1) * ly + my) % arraySize + 1;
            l = std::min(x, y);
            r = std::max(x, y);
            k = ((k - 1) * lk + mk) % (r - l + 1) + 1;
        }
    }

    std::cout << querySum << '\n';
}
