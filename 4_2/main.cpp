/**
 * B. Дискретное логарифмирование
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Даны натуральные числа a, b, и простое число n.
 * Требуется найти дискретный логарифм b по основанию a по модулю n, то есть такое число x (0⩽x<n), что a^x≡b(modn).
 *
 * Входные данные
 * В первой строке заданы через пробел три целых числа a, b и n (0⩽a,b<n⩽1012).
 * Число n - простое.
 *
 * Выходные данные
 * В первой строке выведите −1, если дискретного логарифма не существует.
 * Иначе следует вывести его значение.
 * Если ответ неоднозначен, разрешается выводить любой.
 */

#define DEBUG

#include <assert.h>
#include <cmath>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <unordered_map>

using value_t = unsigned long long;

// a*b mod modulo with regard to overflow
value_t mulMod(value_t a, value_t b, value_t modulo)
{
    value_t result = 0;
    a %= modulo;
    while (b)
    {
        if (b & 1)
        {
            result = (result + a) % modulo;
        }
        a = (2 * a) % modulo;
        b >>= 1;
    }
    return result;
}

// a^x≡b(mod n) -> x
bool calcDiscreteLogarithm(value_t a, value_t b, value_t modulo, value_t& x) {
    value_t n = std::ceil(std::sqrt(modulo));

    value_t base = 1;
    for (value_t i = 0; i < n; ++i)
    {
        base = mulMod(base, a, modulo);
    }

    std::unordered_map<value_t, value_t> vals;
    for (value_t i = 1, e = base; i <= n; ++i)
    {
        auto found = vals.find(e);
        if (found == vals.end())
        {
            vals[e] = i;
        }
        e = mulMod(e, base, modulo);
    }

    for (value_t i = 0, e = b; i <= n; ++i) {
        auto found = vals.find(e);
        if (found != vals.end())
        {
            x = found->second * n - i;
            if (x < modulo)
            {
                return true;
            }
        }
        e = mulMod(e, a, modulo);
    }
    return false;
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

    value_t a, b, n;
    cin >> a >> b >> n;
    assert(n >= 2);
//    assert(gcd(a, n) == 1);
    value_t x;
    if (calcDiscreteLogarithm(a, b, n, x))
    {
        std::cout << x;
    }
    else
    {
        std::cout << "-1";
    }
    return 0;
}
