/**
 * E. Кафе
 *
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Около Петиного университета недавно открылось новое кафе, в котором действует следующая система скидок:
 * при каждой покупке более чем на 100 рублей покупатель получает купон,
 * дающий право на один бесплатный обед (при покупке на сумму 100 рублей и меньше такой купон покупатель не получает).
 * Однажды Пете на глаза попался прейскурант на ближайшие n дней.
 * Внимательно его изучив, он решил, что будет обедать в этом кафе все n дней, причем каждый день он будет покупать в кафе ровно один обед.
 * Однако стипендия у Пети небольшая, и поэтому он хочет по максимуму использовать предоставляемую систему скидок так, чтобы его суммарные затраты были минимальны.
 * Требуется найти минимально возможную суммарную стоимость обедов и номера дней, в которые Пете следует воспользоваться купонами.
 *
 * Входные данные
 * В первой строке входного файла записано целое число n (0 ≤ n ≤ 100).
 * В каждой из последующих n строк записано одно целое число, обозначающее стоимость обеда в рублях на соответствующий день.
 * Стоимость — неотрицательное целое число, не превосходящее 300
 *
 * Выходные данные
 * В первой строке выдайте минимальную возможную суммарную стоимость обедов.
 * Во второй строке выдайте два числа k1 и k2 — количество купонов,
 * которые останутся неиспользованными у Пети после этих n дней и количество использованных им купонов соответственно.
 * В последующих k2 строках выдайте в возрастающем порядке номера дней, когда Пете следует воспользоваться купонами.
 * Если существует несколько решений с минимальной суммарной стоимостью, то выдайте то из них, в котором значение k1 максимально
 * (на случай, если Петя когда-нибудь ещё решит заглянуть в это кафе).
 * Если таких решений несколько, выведите любое из них.
 */

//#define DEBUG

#include <assert.h>
#include <iostream>
#ifdef DEBUG
#include <fstream>
#include <iomanip>
#endif
#include <vector>

class CouponCalculator
{
private:
    static const int COUPON_SUM = 100;
    int sum_;
    std::size_t unusedCoupons_;
    std::vector<std::size_t> couponDays_;
    const int prohibitiveSum_;
    void calc(const std::vector<int>& costs);
public:
    CouponCalculator(const std::vector<int>& costs) : prohibitiveSum_(300 * costs.size() + 1)
    {
        calc(costs);
    }
    friend std::ostream& operator<<(std::ostream& out, const CouponCalculator& value);
};

std::ostream& operator<<(std::ostream& out, const CouponCalculator& value)
{
    out << value.sum_ << '\n' << value.unusedCoupons_ << ' ' << value.couponDays_.size() << '\n';
    for (auto it = value.couponDays_.crbegin(); it < value.couponDays_.crend(); ++it)
    {
        out << (*it + 1) << '\n';
    }
    return out;
}

void CouponCalculator::calc(const std::vector<int>& costs)
{
    enum from_t
    {
        FROM_TOP,
        FROM_TOP_LEFT,
        FROM_TOP_RIGHT
    };

    struct point_t
    {
        int sum_;
        from_t from_;
    };

    const std::size_t size = costs.size();

    if (!size)
    {
        sum_ = 0;
        unusedCoupons_ = 0;
        return;
    }

    point_t* matrix = new point_t[size * (size + 1)]; // days x coupons

    // initialization
    if (costs[0] > COUPON_SUM)
    {
        matrix[0].sum_ = prohibitiveSum_;
        matrix[1].sum_ = costs[0];
    }
    else
    {
        matrix[0].sum_ = costs[0];
        matrix[1].sum_ = prohibitiveSum_;
    }
    for (std::size_t i = 2; i < size + 1; ++i)
    {
        matrix[i].sum_ = prohibitiveSum_;
    }

    // main loop
    for (std::size_t i = 1; i < size; ++i)
    {
        for (std::size_t j = 0; j < size + 1; ++j)
        {
            const int cost = costs[i];
            const std::size_t coord = (size + 1) * i + j;
            const std::size_t couponUsedCoord = (size + 1) * (i - 1) + (j + 1);
            assert(couponUsedCoord < size * size);
            const std::size_t couponNotUsedCoord = (size + 1) * (i - 1) + (cost > COUPON_SUM && j ? j - 1 : j);
            assert(couponUsedCoord < size * size);
            const from_t couponNotUsedFrom = cost > COUPON_SUM && j ? FROM_TOP_LEFT : FROM_TOP;
            const int sumCouponUsed = j == size ? prohibitiveSum_ : matrix[couponUsedCoord].sum_;
            const int sumCouponNotUsed = matrix[couponNotUsedCoord].sum_ + cost;
            if (sumCouponUsed < sumCouponNotUsed)
            {
                matrix[coord].sum_ = sumCouponUsed;
                matrix[coord].from_ = FROM_TOP_RIGHT;
            }
            else
            {
                matrix[coord].sum_ = sumCouponNotUsed;
                matrix[coord].from_ = couponNotUsedFrom;
            }
        }
    }

#ifdef DEBUG
    for (std::size_t i = 0; i < size; ++i)
    {
        for (std::size_t j = 0; j < size + 1; ++j)
        {
            std::cout << std::setw(7) << matrix[i * (size + 1) + j].sum_;
            switch (matrix[i * (size + 1) + j].from_) {
                case FROM_TOP:
                    std::cout << " T| ";
                break;
                case FROM_TOP_LEFT:
                    std::cout << " L| ";
                break;
                case FROM_TOP_RIGHT:
                    std::cout << " R| ";
                break;
            default:
                std::cout << " D| ";
            }
        }
        std::cout << std::endl;
    }
#endif
    // decypher the answer
    std::size_t index = (size + 1) * size - 1;
    sum_ = matrix[index].sum_;
    for (std::size_t i = 1; i < size + 1; ++i)
    {
        if (matrix[(size + 1) * size - i - 1].sum_ < sum_)
        {
           index = (size + 1) * size - i - 1;
           sum_ = matrix[index].sum_;
        }
    }
    unusedCoupons_ = index - (size + 1) * size + size + 1;

    for (std::size_t i = size - 1; i > 0; --i)
    {
        switch (matrix[index].from_) {
            case FROM_TOP:
                index -= size + 1;
            break;
            case FROM_TOP_LEFT:
                index -= size + 2;
            break;
            case FROM_TOP_RIGHT:
                index -= size;
                couponDays_.push_back(i);
            break;
        }
    }
    assert(costs[0] > COUPON_SUM ? index == 1 : index == 0);

    delete[] matrix;
}

int main()
{
#ifdef DEBUG
    std::ifstream cin("input.txt");
#else
    using std::cin;
#endif

    std::size_t count;
    cin >> count;
    std::vector<int> costs(count);
    for (std::size_t i = 0; i < count; ++i)
    {
        cin >> costs[i];
    }

    std::cout << CouponCalculator(costs);

    return 0;
}
