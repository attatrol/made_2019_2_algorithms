/**
  * A. Задача для второклассника
  *
  * ввод стандартный ввод
  * вывод стандартный вывод
  *
  * Вам даны два числа.
  * Необходимо найти их произведение.
  *
  * Входные данные
  * Входные данные состоят из двух строк, на каждой из которых находится целое одно целое число, длина которого не превосходит двухсот пятидесяти тысяч символов.
  *
  * Выходные данные
  * Выведите произведение данных чисел.
  */

#define DEBUG
#define _USE_MATH_DEFINES

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <complex>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif
#include <vector>

template <typename T> class Number;

template <typename T>
std::ostream& operator<<(std::ostream& os, const Polynom<T>& number);
template <typename T>
Polynom<T> operator*(const Polynom<T>& lhs, const Polynom<T>& rhs);

template <typename T>
class Number
{
private:
    bool sign_;
    std::vector<T> digits_;

    Number(std::vector<T> digits, bool sign) : sign_(sign), digits_(digits)
    {
    }
public:
    Number(const std::string& str)
    {
        std::size_t offset = 0;
        if (str[0] == '-')
        {
            sign_ = false;
            ++offset;
        }
        else
        {
            sign_ = true;
        }
        digits_.resize(str.size() - offset);
        for (std::size_t i = 0, limit = str.size() - offset; i < limit; ++i)
        {
            assert(std::isdigit(str[i + offset]));
            digits_[limit - i - 1] = str[i + offset] - '0';
        }
    }
    friend std::ostream& operator<<<>(std::ostream& os, const Number<T>& number);
    friend Number<T> operator*<>(const Number<T>& lhs, const Number<T>& rhs);
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Polynom<T>& number)
{
    bool trailingZero = true;
    for (auto it = number.digits_.rbegin(); it < number.digits_.rend(); ++it)
    {
        if (*it == 0 && trailingZero)
        {
            continue;
        }
        if (trailingZero)
        {
            if (!number.sign_)
            {
                os << '-';
            }
            trailingZero = false;
        }
        os << *it;
    }
    if (trailingZero)
    {
        os << '0';
    }
}

void fft(std::vector<std::complex<double>>& arg, bool invert)
{
    std::size_t size = arg.size();
    if (size == 1)
    {
        return;
    }

    std::vector<std::complex<double>> even(size / 2);
    std::vector<std::complex<double>> odd(size / 2);
    for (std::size_t i = 0, j = 0; i < size; i += 2, ++j)
    {
        even[j] = arg[i];
        odd[j] = arg[i + 1];
    }

    fft(even, invert);
    fft(odd, invert);

    double ang = 2 * M_PI / size * (invert ? -1 : 1);
    std::complex<double> w(1);
    std::complex<double> wn(std::cos(ang), std::sin(ang));
    for (std::size_t i = 0; i < size / 2; ++i)
    {
        arg[i] = even[i] + w * odd[i];
        arg[i + size / 2] = even[i] - w * odd[i];
        if (invert)
        {
            arg[i] /= 2;
            arg[i + size / 2] /= 2;
        }
        w *= wn;
    }
}

template <typename T>
Polynom<T> operator *(const Polynom<T>& lhs, const Polynom<T>& rhs)
{
    std::vector<std::complex<double>> left(lhs.digits_.begin(), lhs.digits_.end());
    std::vector<std::complex<double>> right(rhs.digits_.begin(), rhs.digits_.end());

    const std::size_t maxArgSize = std::max(left.size(), right.size());
    std::size_t resultSize = 1;
    while (resultSize < maxArgSize)
    {
        resultSize <<= 1;
    }
    resultSize <<= 1;
    left.resize(resultSize);
    right.resize(resultSize);

    fft (left, false);
    fft (right, false);
    for (size_t i = 0; i < resultSize; ++i)
    {
        left[i] *= right[i];
    }
    fft (left, true);

    std::vector<T> digits(resultSize);
    T carry = 0;
    for (std::size_t i = 0; i < resultSize; ++i)
    {
        digits[i] = static_cast<T>(left[i].real() + 0.5) + carry;
        carry = digits[i] / 10;
        digits[i] %= 10;
    }
    assert(carry == 0);
    const bool sign = lhs.sign_ == rhs.sign_;

    return Polynom<T>(digits, sign);
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
    std::string str0, str1;
    cin >> str0 >> str1;

    Polynom<int> num0(str0);
    Polynom<int> num1(str1);

    Polynom<int> product = num0 * num1;

    std::cout << product;

    return 0;
}

