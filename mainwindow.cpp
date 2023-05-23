#include "mainwindow.h"
#include "rand.h"
#include <QDebug>
#include <cmath>
#include "qfunc.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    plot.setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    plot.axisRect()->setupFullAxesBox(true);
    plot.xAxis->setLabel("x");
    plot.yAxis->setLabel("y");

    start();

    plot.legend->setVisible(true);
    resize(700, 600);
    setCentralWidget(&plot);
}

namespace details {
v<qreal> get_start(qreal len) noexcept {
    qreal angle = random(0, M_PI * 2, 2);
    return {std::cos(angle) * len, std::sin(angle) * len};
}
}

void MainWindow::start()
{
    // testing functions
    func_t f1 = [](qreal const x) {
        return 0.1 * x * x;
    };
    func_t f2 = f1;

    // making solution way
    v<qreal> key;
    v<qreal> value_const;
    v<qreal> value_optimal;
    v<qreal> point;

    for (int i = 0; i < 10000; ++i) {
        point = details::get_start(i);
        key.push_back(i);
        value_const.push_back(gradient_decent({f1, f2}, point, 0.1, 1000).size());
        value_optimal.push_back(gradient_decent_with_dihotomy({f1, f2}, point, 0.1, 1000).size());
    }


    // representing data
//    set_color_map({std::min(way[0][0], way.back()[0]) - 10, std::min(way[0][1], way.back()[1]) - 10},
//                  {std::max(way[0][0], way.back()[0]) + 10, std::max(way[0][1], way.back()[1]) + 10},
//                  {500, 500}, f1, f2);

    plot.addGraph();
    plot.graph()->setPen(QPen(QBrush(Qt::blue),2));
    plot.graph()->setName("Optimal");
    plot.graph()->addData(key, value_optimal);

    plot.addGraph();
    plot.graph()->setPen(QPen(QBrush(Qt::red),2));
    plot.graph()->setName("Usual");
    plot.graph()->addData(key, value_const);

    plot.rescaleAxes();

//    make_way(way);
}

qreal MainWindow::grad(const func_t &f, const qreal x) noexcept
{
    return (f(x + EPSILON) - f(x)) / EPSILON;
}

qreal MainWindow::next_step(const func_t &f, qreal const x, qreal const lr) noexcept
{
    return x - lr * grad(f, x);
}

qreal MainWindow::func_sum(const v<func_t> & funcs, v<qreal> const& pos) noexcept
{
    qreal result{0};
    for (int i = 0; i < pos.size(); ++i) {
        result += funcs[i](pos[i]);
    }
    return result;
}

v<v<qreal>> MainWindow::gradient_decent(const v<func_t> & funcs, v<qreal> const& start, const qreal lr, int max_step) noexcept
{
    v<v<qreal>> way;
    int i;
    way.push_back(start);

    int const metric = start.size();
    v<qreal> cur_pos = std::move(start);
    v<qreal> old_pos = cur_pos;

    for (i = 0; i < old_pos.size(); ++i) old_pos[i] -= EPSILON * 100;
    while (max_step-- > 0) {
        old_pos = cur_pos;

        for (i = 0; i < metric; ++i) {
            cur_pos[i] = next_step(funcs[i], cur_pos[i], lr);
        }
        if ((std::abs(func_sum(funcs, old_pos) - func_sum(funcs, cur_pos)) <= EPSILON)) {
            break;
        }
        way.push_back(cur_pos);
    }
    return way;
}

v<v<qreal> > MainWindow::gradient_decent_with_dihotomy(const v<func_t> & funcs, v<qreal> const& start, qreal const lr, int const max_step) noexcept
{
    v<qreal> a(start.size(), 0);
    v<qreal> b(start.size(), 0);
    qreal m;
    qreal grd;

    for (int i = 0; i < start.size(); ++i) {
        m = lr;
        grd = grad(funcs[i], start[i]);
        grd = grd / std::abs(grd);

        a[i] = start[i];
        while (grad(funcs[i], start[i] - m * grd) * grd >= 0) {
            m *= 2;
        }
        b[i] = start[i] - m * grd;

        if (a[i] > b[i]) std::swap(a[i], b[i]);
    }

    return dihotomy(funcs, std::move(a), std::move(b), start, max_step);
}

v<v<qreal>> MainWindow::dihotomy(const v<func_t> & funcs, v<qreal>&& a, v<qreal>&& b, v<qreal> const& start, int max_step) noexcept
{
    int i;
    v<v<qreal>> way = {std::move(start)};
    int const metric = a.size();
    v<qreal> x(metric, 0);
    v<bool> final(metric, false);
    qreal grd{0};
    bool contin = true;

    while (max_step-- > 0 && contin) {
        contin = false;

        for (i = 0; i < metric; ++i) {
            if (final[i]) {
                continue;
            }
            x[i] = (a[i] + b[i]) / 2;
            grd = grad(funcs[i], x[i]);
            if (std::abs(grd) < EPSILON) {
                final[i] = true;
            } else {
                if (grad(funcs[i], x[i]) > 0) {
                    b[i] = x[i];
                } else {
                    a[i] = x[i];
                }
            }
            contin |= !final[i];
        }
        way.push_back(x);
    }
    return way;
}

void MainWindow::make_graph(qreal x_left, qreal x_right, size_t resolution, const func_t & f)
{
    QVector<qreal> key, value;
    qreal scale = (x_right - x_left) / resolution;

    key.reserve(resolution);
    value.reserve(resolution);
    for (size_t i = 0; i < resolution; ++i) {
        key.push_back(x_left + (scale * i));
        value.push_back(f(key.back()));
    }
    plot.addGraph();
    plot.graph()->setData(key, value);
    plot.graph()->setPen(QPen(QBrush(Qt::blue), 2));
    plot.graph()->setName("График");
    plot.rescaleAxes();
}

void MainWindow::set_color_map(QPointF const& left_bottom, QPointF const& right_top,
                               QSize const& resolution,
                               auto const& f)
{
    QCPColorMap* color_map = new QCPColorMap(plot.xAxis, plot.yAxis);
    plot.legend->removeItem(0);
    color_map->data()->setSize(resolution.width(), resolution.height());
    color_map->data()->setRange(QCPRange(left_bottom.x(), right_top.x()),
                                QCPRange(left_bottom.y(), right_top.y()));
    color_map->setTightBoundary(true);
    qreal dx, dy;

    // set heights
    QPoint cur{0, 0};
    for (cur.ry() = 0; cur.y() < resolution.height(); ++cur.ry()) {
        for (cur.rx() = 0; cur.x() < resolution.width(); ++cur.rx()) {
            color_map->data()->cellToCoord(cur.x(), cur.y(), &dx, &dy);
            color_map->data()->setCell(cur.x(), cur.y(), f(dx, dy));
        }
    }

    //set gradient
    QCPColorScale* color_scale = new QCPColorScale(&plot);
    color_map->setInterpolate(false);
    plot.plotLayout()->addElement(0, 1, color_scale);
    color_scale->setType(QCPAxis::atRight);
    color_scale->axis()->setLabel("Iterations");



    color_map->setColorScale(color_scale);
    color_map->rescaleDataRange();

    QCPColorGradient grad(QCPColorGradient::gpGrayscale);
    //grad.setLevelCount(17);
    color_map->setGradient(grad);


    QCPMarginGroup *marginGroup = new QCPMarginGroup(&plot);
    plot.axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    color_scale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    color_map->rescaleAxes();
}

void MainWindow::make_way(v<v<qreal>> const& way)
{
    plot.addGraph();
    plot.graph()->setPen(QPen(QBrush(Qt::blue),2));
    plot.graph()->setName("Way");
    for (int i = 0; i < way.size(); ++i) {
        plot.graph()->addData(way[i][0], way[i][1]);
    }
    plot.addGraph();
    plot.graph()->addData(way[0][0], way[0][1]);
    plot.graph()->setName("Start");
    plot.graph()->setPen(QPen(QBrush(),0));

    QCPScatterStyle sc_begin(QCPScatterStyle::ssCross);
    sc_begin.setPen(QPen(QBrush(Qt::green),2));
    plot.graph()->setScatterStyle(sc_begin);

    plot.addGraph();
    plot.graph()->addData(way.back()[0], way.back()[1]);
    plot.graph()->setName("End");
    plot.graph()->setPen(QPen(QBrush(),0));

    QCPScatterStyle sc_end(QCPScatterStyle::ssTriangle);
    sc_end.setPen(QPen(QBrush(Qt::red),2));
    plot.graph()->setScatterStyle(sc_end);
}

/*
 * (0, 0) (0, 1)
*/
