/**
 * C. Cambio
 *
 * input standard input
 * output standard output
 *
 * You are given two strings s and t of the same length m=2n for some n≥2.
 * Find cyclic shift of string t maximizing number of coincident symbols.
 * More formally, find x such that number of i with s[i]=t[(i−x)mod m] is maximized.
 *
 * Input
 * The first line contains one integer 4≤m≤131072.
 * Next two lines contains strings of length m, consisting of symbols 'A', 'C', 'G', 'T'.
 *
 * Output
 * Output two integer — maximum number of coincident symbols and number of symbols to move from the end of string t to its beginning to achieve the result.
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

template <typename T> class Polynom;

template <typename T>
std::ostream& operator<<(std::ostream& os, const Polynom<T>& number);
template <typename T>
Polynom<T> operator*(const Polynom<T>& lhs, const Polynom<T>& rhs);

template <typename T>
class Polynom
{
private:
    std::vector<T> digits_;

public:
    Polynom(std::vector<T> digits) :digits_(digits)
    {
    }
    const std::vector<T>& digits() const
    {
        return digits_;
    }
    friend std::ostream& operator<<<>(std::ostream& os, const Polynom<T>& number);
    friend Polynom<T> operator*<>(const Polynom<T>& lhs, const Polynom<T>& rhs);
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Polynom<T>& number)
{
    for(std::size_t i = number.digits_.size() - 1; i < number.digits_.size(); --i)
    {
        std::cout << i << ' ' << number.digits_[i] << '\n';
    }
    return os;
}

void fft(std::vector<std::complex<double>>& arg, bool invert)
{
    std::size_t size = arg.size();

    for (std::size_t i = 1, j = 0; i < size; ++i)
    {
        std::size_t bit = size >> 1;
        for (; j >= bit; bit >>= 1)
        {
            j -= bit;
        }
        j += bit;
        if (i < j)
        {
            std::swap(arg[i], arg[j]);
        }
    }

    for (std::size_t length = 2; length <= size; length <<= 1)
    {
        const double angle = 2 * M_PI / length * (invert ? -1 : 1);
        std::complex<double> wlen(cos(angle), sin(angle));
        for (std::size_t i = 0; i < size; i += length)
        {
            std::complex<double> w (1);
            for (std::size_t j = 0; j < length / 2; ++j)
            {
                std::complex<double> u = arg[i+j],  v = arg[i+j+length/2] * w;
                arg[i + j] = u + v;
                arg[i + j + length / 2] = u - v;
                w *= wlen;
            }
        }
    }
    if (invert)
    {
        for (std::size_t i = 0; i<size; ++i)
        {
            arg[i] /= size;
        }
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
    for (std::size_t i = 0; i < resultSize; ++i)
    {
        digits[i] = static_cast<T>(left[i].real() + 0.5);
    }

    return Polynom<T>(digits);
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
    std::size_t dnaSize;
    std::string dna0, dna1;
    cin >> dnaSize >> dna0 >> dna1;

    std::vector<char> alphabet = {'A', 'C', 'G', 'T'};
    std::vector<int> cyclicDotProducts(dnaSize);
    for (char ch : alphabet)
    {
        std::vector<int> vector0(2 * dnaSize);
        for (std::size_t i = 0; i < dnaSize; ++i)
        {
            if (dna0[i] == ch)
            {
                vector0[2 * dnaSize - i - 1] = 1;
            }
        }
        std::vector<int> vector1(2 * dnaSize);
        for (std::size_t i = 0; i < dnaSize; ++i)
        {
            if (dna1[i] == ch)
            {
                vector1[i] = 1;
                vector1[i + dnaSize] = 1;
            }
        }

        Polynom<int> poly0(vector0);
        Polynom<int> poly1(vector1);

        Polynom<int> product = poly0 * poly1;

        for (std::size_t i = 0; i < dnaSize; ++i)
        {
            cyclicDotProducts[i] += product.digits().at(i + 2 * dnaSize);
        }
    }

    std::size_t maxDotIndex = dnaSize - 1;
    int maxDotValue = cyclicDotProducts[maxDotIndex];
    for (std::size_t i = dnaSize - 2; i < dnaSize; --i)
    {
        if (maxDotValue < cyclicDotProducts[i])
        {
            maxDotIndex = i;
            maxDotValue = cyclicDotProducts[i];

        }
    }

    std::cout << maxDotValue << ' ' << (dnaSize - maxDotIndex - 1) << '\n';

    return 0;
}

