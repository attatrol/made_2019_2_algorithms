#include<iostream>
#include<algorithm>
#include<cstring>
#include<cstdio>
#include<cmath>
#include<vector>

using namespace std;

typedef double db;

const int maxn = 2e5, maxm = 1 << 19;
const db pi = acos(-1);
int n, m, L, M[200], a[5][maxn + 3], b[5][maxn + 3], c[5][maxn + 3], k, rev[maxm + 3];
char s[maxn + 3];

struct complex {
    db r, i;
    complex(db r = 0, db i = 0): r(r), i(i) {}
    friend complex operator+ (const complex &a, const complex &b) {
        return complex(a.r + b.r, a.i + b.i);
    }

    friend complex operator- (const complex &a, const complex &b) {
        return complex(a.r - b.r, a.i - b.i);
    }

    friend complex operator* (const complex &a, const complex &b) {
        return complex(a.r * b.r - a.i * b.i, a.r * b.i + a.i * b.r);
    }

    friend complex operator/ (const complex &a, const db &b) {
        return complex(a.r / b, a.i / b);
    }

};

complex A[maxm + 3], B[maxm + 3], C[maxm + 3];

void dft(complex a[], int n, int type) {
    for (int i = 0; i < n; i++) if (i < rev[i]) {
        swap(a[i], a[rev[i]]);
    }
    for (int k = 1; k < n; k *= 2) {
        complex x = complex(cos(pi / k), type * sin(pi / k));
        for (int i = 0; i < n; i += k * 2) {
            complex y = 1;
            for (int j = i; j < i + k; j++, y = x * y) {
                complex p = a[j], q = a[j + k] * y;
                a[j] = p + q, a[j + k] = p - q;
            }
        }
    }
    if (type == -1) {
        for (int i = 0; i < n; i++)
            a[i] = a[i] / n;
        }
    }
}

inline int id(char ch) {

    return M[ch];

}

int main() {
    M['A'] = 0, M['T'] = 1, M['G'] = 2, M['C'] = 3;
    scanf("%d %d %d %s", &n, &m, &L, s + 1);
    for (int i = 1; i <= n; i++)
    {
        int x = id(s[i]), l = max(1, i - L), r = min(n, i + L);
        a[x][l - 1]++, a[x][r]--;
    }

    for (int k = 0; k < 4; k++)
    {
        for (int i = 1; i < n; i++)
        {
            a[k][i] += a[k][i - 1];
        }
        for (int i = 0; i < n; i++)
        {
            a[k][i] = a[k][i] > 0;

        }
    }

    scanf("%s", s + 1);

    for (int i = 1; i <= m; i++)
    {
        b[id(s[i])][m - i] = 1;
    }

    for (k = 0; 1 << k <= n + m - 2; k++);

    for (int i = 1; i < 1 << k; i++)
    {
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (k - 1));
    }

    for (int t = 0; t < 4; t++)
    {
        int sum = 0;
        for (int i = 0; i <= n - 1; i++)
        {
            A[i] = a[t][i];
        }
        for (int i = n; i < 1 << k; i++)
        {
            A[i] = 0;
        }

        for (int i = 0; i <= m - 1; i++)
        {
            B[i] = b[t][i], sum += b[t][i];
        }

        for (int i = m; i < 1 << k; i++)
        {
            B[i] = 0;
        }
        dft(A, 1 << k, 1);
        dft(B, 1 << k, 1);

        for (int i = 0; i < 1 << k; i++)
        {
            C[i] = A[i] * B[i];
        }
        dft(C, 1 << k, -1);
        for (int i = m - 1; i < n; i++)
        {
            int x = C[i].r + .5;
            if (x == sum)
            {
                c[t][i + 1] = 1;
            }
        }
    }

    int ans = 0;

    for (int i = m; i <= n; i++)
    {
        if (c[0][i] && c[1][i] && c[2][i] && c[3][i])
        {
            ans++;
        }
    }

    printf("%d\n", ans);

    return 0;

}
