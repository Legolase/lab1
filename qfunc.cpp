#include "qfunc.h"
#include "rand.h"
#include <QDebug>


constexpr qfunc::qfunc(const qreal a_, const qreal b_, const qreal c_) noexcept : a(a_), b(b_), c(c_)
{

}

constexpr qreal qfunc::operator()(const qreal x) const noexcept {
    return a * x * x + b * x + c;
}

v<std::function<qreal (qreal)> > generate_func(int n, const qreal k)
{
    v<std::function<qreal(qreal)>> result;
    qreal min_ = random(1., 10., 5);
    qreal max_ = min_ * k;
    if (max_ < min_) {
        std::swap(max_, min_);
    }
    qfunc temp(0, 0, 0);
    while (n-- > 0) {
        temp.a = random(min_, max_, 5);
        temp.b = random(-1000000., 1000000., 5);
        temp.c = random(-1000000., 1000000., 5);
        result.push_back(temp);
    }
    return result;
}
