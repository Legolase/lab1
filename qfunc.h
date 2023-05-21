#ifndef QFUNC_H
#define QFUNC_H

#include <QtGlobal>
#include <QVector>
#include <functional>

template<typename T>
using v = QVector<T>;

struct qfunc
{
    qreal a;
    qreal b;
    qreal c;

    constexpr qfunc(qreal const a_, qreal const b_, qreal const c_) noexcept;
    constexpr qfunc(qfunc const&) = default;

    constexpr qreal operator()(qreal const x) const noexcept;
};

v<std::function<qreal(qreal)>> generate_func(int n, qreal const k);

#endif // QFUNC_H
