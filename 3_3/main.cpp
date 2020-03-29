/**
 * C. Взлом RSA
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * В 1977 году Ronald Linn Rivest, Adi Shamir и Leonard Adleman предложили новую криптографическую схему RSA, используемую до сих пор.
 * RSA является криптосистемой с открытым ключом: зашифровать сообщение может кто угодно, знающий общеизвестный открытый ключ, а расшифровать сообщение — только тот, кто знает специальный секретный ключ.
 *
 * Желающий использовать систему RSA для получения сообщений должен сгенерировать два простых числа p и q, вычислить n = pq и сгенерировать два числа e и d такие, что {ed ≡ 1 ± od{(p - 1)(q - 1)}} (заметим, что {(p - 1)(q - 1) = φ(n)}).
 * Числа n и e составляют открытый ключ и являются общеизвестными.
 * Число d является секретным ключом, также необходимо хранить в тайне и разложение числа n на простые множители, так как это позволяет вычислить секретный ключ d.
 * Сообщениями в системе RSA являются числа из кольца Zn.
 * Пусть M — исходное сообщение.
 * Для его шифрования вычисляется значение С = M^e mod n (для этого необходимо только знание открытого ключа).
 * Полученное зашифрованное сообщение C передается по каналу связи.
 * Для его расшифровки необходимо вычислить значение , а для этого необходимо знание секретного ключа.
 * Вы перехватили зашифрованное сообщение C и знаете только открытый ключ: числа n и e.
 * "Взломайте" RSA — расшифруйте сообщение на основе только этих данных.
 *
 * Входные данные
 * Программа получает на вход три натуральных числа: n, e, C, n ≤ 10^9, e ≤ 10^9, C < n.
 * Числа n и e являются частью какой-то реальной схемы RSA, т.е. n является произведением двух простых и e взаимно просто с φ(n).
 * Число C является результатом шифрования некоторого сообщения M.
 *
 * Выходные данные
 * Выведите одно число M (0 ≤ M < n), которое было зашифровано такой криптосхемой.
 */

#define DEBUG

#include <assert.h>
#include <cmath>
#include <iostream>
#ifdef DEBUG
#include <chrono>
#include <fstream>
#endif

using ull_t = unsigned long long;

std::pair<int, int> factorize(int n)
{
    if (n % 2 == 0)
    {
        return std::make_pair(2, n / 2);
    }
    for (int i = 3, limit = (int)std::ceil(std::sqrt(n)); i <= limit; i += 2)
    {
        if (n % i == 0)
        {
            return std::make_pair(i, n / i);
        }
    }
    assert(false);
}

// ax + by == gcd(a, b)
int gcd(int a, int b, int& x, int& y) {
    if (a == 0)
    {
        x = 0;
        y = 1;
        return b;
    }
    int x1, y1;
    int d = gcd(b%a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
    return d;
}

int calculateD(const int e, const int totient)
{
    // de = 1 mod totient
    assert(totient > e ? totient % e != 0 : e % totient != 0);

    int x, y;
    int g = gcd(e, totient, x, y);
    assert(g == 1);
    x = (x % totient + totient) % totient;
    return x;
}

// calculate value^exp mod modulo
ull_t expMod(ull_t value, ull_t exp, ull_t modulo)
{
    ull_t res = 1;

    while (exp != 0)
    {
        if ((exp & 1) != 0)
        {
            res = (res * value) % modulo;
        }

        value = (value * value) % modulo;
        exp >>= 1;
    }

    return res;
}

ull_t decrypt(const int n, const int d, const int C)
{
    return expMod(C, d, n);
}

ull_t bruteRCA(int n, int e, int C)
{
#ifdef DEBUG
    auto t0 = std::chrono::high_resolution_clock::now();
#endif
    std::pair<int, int> pq = factorize(n);
#ifdef DEBUG
    auto t1 = std::chrono::high_resolution_clock::now();
    auto d0 = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    std::cout << "duration " << d0 << '\n';

    auto t2 = std::chrono::high_resolution_clock::now();
#endif
    int totient = (pq.first - 1) * (pq.second - 1);
    int d = calculateD(e, totient);
#ifdef DEBUG
    auto t3 = std::chrono::high_resolution_clock::now();
    auto d1 = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
    std::cout << "calculateD " << d1 << '\n';

    auto t4 = std::chrono::high_resolution_clock::now();
#endif
    auto result = decrypt(n, d, C);
#ifdef DEBUG
    auto t5 = std::chrono::high_resolution_clock::now();
    auto d2 = std::chrono::duration_cast<std::chrono::microseconds>(t5 - t4).count();
    std::cout << "decrypt " << d2 << '\n';
#endif
    return result;
}

int main()
{
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
    std::ios::sync_with_stdio(false);
    cin.tie(0);
    std::cout.tie(0);
#endif

    int n, e, C;
    cin >> n >> e >> C;
    std::cout << bruteRCA(n, e, C) << std::endl;

    return 0;
}
