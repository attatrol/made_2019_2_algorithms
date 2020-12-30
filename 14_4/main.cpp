/**
 * D. Fuzzy Search
 *
 * input standard input
 * output standard output
 *
 * Leonid works for a small and promising start-up that works on decoding the human genome.
 * His duties include solving complex problems of finding certain patterns in long strings consisting of letters 'A', 'T', 'G' and 'C'.
 * Let's consider the following scenario.
 * There is a fragment of a human DNA chain, recorded as a string S.
 * To analyze the fragment, you need to find all occurrences of string T in a string S.
 * However, the matter is complicated by the fact that the original chain fragment could contain minor mutations, which, however, complicate the task of finding a fragment.
 * Leonid proposed the following approach to solve this problem.
 * Let's write down integer k ≥ 0 — the error threshold.
 * We will say that string T occurs in string S on position i (1 ≤ i ≤ |S| - |T| + 1),
 * if after putting string T along with this position,
 * each character of string T corresponds to the some character of the same value in string S at the distance of at most k.
 * More formally, for any j (1 ≤ j ≤ |T|) there must exist such p (1 ≤ p ≤ |S|), that |(i + j - 1) - p| ≤ k and S[p] = T[j].
 * For example, corresponding to the given definition, string "ACAT" occurs in string "AGCAATTCAT" in positions 2, 3 and 6.
 * Note that at k = 0 the given definition transforms to a simple definition of the occurrence of a string in a string.
 * Help Leonid by calculating in how many positions the given string T occurs in the given string S with the given error threshold.
 *
 * Input
 * The first line contains three integers |S|, |T|, k (1 ≤ |T| ≤ |S| ≤ 200 000, 0 ≤ k ≤ 200 000) — the lengths of strings S and T and the error threshold.
 * The second line contains string S.
 * The third line contains string T.
 * Both strings consist only of uppercase letters 'A', 'T', 'G' and 'C'.
 *
 * Output
 * Print a single number — the number of occurrences of T in S with the error threshold k by the given definition.
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
            std::complex<double> w(1);
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
    size_t dna0Length, dna1Length, errorFactor;
    std::string dna0, dna1;
    cin >> dna0Length >> dna1Length >> errorFactor >> dna0 >> dna1;

    std::vector<std::size_t> matches(dna0Length);
    const std::vector<char> chars = { 'A', 'C', 'G', 'T' };
    for (char ch : chars)
    {
        std::vector<int> projection0(dna0Length);
        std::vector<int> projection1(dna1Length);
        for (std::size_t i = 0; i < dna0Length; ++i)
        {
            if (dna0[i] == ch)
            {
                projection0[std::max(i, errorFactor) - errorFactor] += 1;
                if (i + errorFactor + 1 < dna0Length)
                {
                    projection0[i + errorFactor + 1] -= 1;
                }
            }
        }
        for (std::size_t i = 1; i < dna0Length; ++i)
        {
            projection0[i] += projection0[i - 1];
        }
        for (std::size_t i = 0; i < dna0Length; ++i)
        {
            projection0[i] = projection0[i] > 0;
        }
        for (std::size_t i = 0; i < dna1Length; ++i)
        {
            projection1[dna1Length - i - 1] = dna1[i] == ch;
        }

        Polynom<int> poly0(projection0);
        Polynom<int> poly1(projection1);
        auto product = poly0 * poly1;
        auto& dotProducts = product.digits();
        for (std::size_t k = 0; k < dna0Length; ++k)
        {
            matches[k] += dotProducts[k + dna1Length - 1];
        }
    }
    std::size_t entryCount = 0;
    for (std::size_t i = 0; i < dna0Length; ++i)
    {
        entryCount += matches[i] == dna1Length;
    }
    std::cout << entryCount;

    return 0;
}
