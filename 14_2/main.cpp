/**
  * B. Дуэль
  * ввод стандартный ввод
  * вывод стандартный вывод
  *
  * Двое дуэлянтов решили выбрать в качестве места проведения поединка тёмную аллею.
  * Вдоль этой аллеи растёт n деревьев и кустов.
  * Расстояние между соседними объектами равно одному метру.
  * Дуэль решили проводить по следующим правилам.
  * Некоторое дерево выбирается в качестве стартовой точки.
  * Затем два дерева, находящихся на одинаковом расстоянии от исходного, отмечаются как места для стрельбы.
  * Дуэлянты начинают движение от стартовой точки в противоположных направлениях.
  * Когда соперники достигают отмеченных деревьев, они разворачиваются и начинают стрелять друг в друга.
  *
  * Дана схема расположения деревьев вдоль аллеи.
  * Требуется определить количество способов выбрать стартовую точку и места для стрельбы согласно правилам дуэли.
  *
  * Входные данные
  * Во входном файле содержится одна строка, состоящая из символов '0' и '1' — схема аллеи.
  * Деревья обозначаются символом '1', кусты — символом '0'.
  * Длина строки не превосходит 100000 символов.
  *
  * Выходные данные
  * Выведите количество способов выбрать стартовую точку и места для стрельбы согласно правилам дуэли.
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

    Polynom(std::vector<T> digits) :digits_(digits)
    {
    }
public:
    Polynom(const std::string& str)
    {
        digits_.resize(str.size());
        for (std::size_t i = 0, limit = str.size(); i < limit; ++i)
        {
            assert(std::isdigit(str[i]));
            digits_[limit - i - 1] = str[i] - '0';
        }
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
    std::string str;
    cin >> str;

    Polynom<int> sequence(str);
    Polynom<int> product = sequence * sequence;

    auto sequenceDigits = sequence.digits();
    auto productDigits = product.digits();

    long long duelCounts = 0;
    for(std::size_t i = 0; i < sequenceDigits.size(); ++i)
    {
        if (sequenceDigits[i])
        {
            duelCounts += (productDigits[2 * i] - 1) / 2;
        }
    }

    std::cout << duelCounts << '\n';

    return 0;
}

