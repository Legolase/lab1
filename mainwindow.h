#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include <cmath>
#include <functional>
#include <QVector>

template<typename T>
using v = QVector<T>;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    static constexpr qreal EPSILON = 10.e-10;

    using func_t = std::function<qreal(qreal)>;
public:
    MainWindow(QWidget *parent = nullptr);
private:
    void start();

    static qreal grad(func_t const& f, qreal const x) noexcept;
    static qreal next_step(func_t const& f, qreal const x, qreal const lr) noexcept;
    static v<v<qreal>> gradient_decent(v<func_t> const&, v<qreal> const& start, qreal const lr, int max_step) noexcept;
    static v<v<qreal>> gradient_decent_with_dihotomy(v<func_t> const&, v<qreal>const& start, qreal const lr, int const max_step) noexcept;
    static v<v<qreal>> dihotomy(v<func_t> const&, v<qreal>&& a, v<qreal>&& b, v<qreal> const& start, int max_step) noexcept;
    static qreal func_sum(v<func_t> const&, v<qreal>const& pos) noexcept;

    void make_graph(qreal x_left, qreal x_right,  size_t resolution, func_t const&);
    void set_color_map(QPointF const& left_bottom, QPointF const& right_top,
                       QSize const& resolution,
                       auto const& f);
    void make_way(v<v<qreal>>const& way);

    QCustomPlot plot{};
};

#endif // MAINWINDOW_H
