#include "bezierwidget.h"

#include <math.h>

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QList>
#include <QLineF>

#include <QMouseEvent>

#include <QDebug>

BezierCurveEditWidget::BezierCurveEditWidget(float p,
                                             float s,
                                             float g,
                                             float r,
                                             double size,
                                             float text,
                                             double gap,
                                             int samples,
                                             QWidget *parent)
    : QGraphicsView(parent),
      _primary_line_width(p),
      _secondary_line_width(s),
      _grid_line_width(g),
      _radius(r),
      _size_scale(size),
      _text_scale(text),
      _gap(gap),
      _samples(samples)
{
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setSceneRect(0.00, 0.00, this->_size_scale, this->_size_scale);
    this->setScene(scene);
    this->fitInView(-this->_gap, -this->_gap, this->_size_scale + this->_gap, this->_size_scale + this->_gap, Qt::KeepAspectRatio);

    this->setMouseTracking(true);
    this->viewport()->setMouseTracking(true);

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QPen pen(QColor(80, 80, 80, 100));
    pen.setWidth(this->_grid_line_width);

    for (int i = 1; i < 5; i++)
    {
        scene->addLine(0.20 * this->_size_scale * (double)i, 0.00, 0.20 * this->_size_scale * (double)i, this->_size_scale, pen);
        scene->addLine(0.00, 0.20 * this->_size_scale * (double)i, this->_size_scale, 0.20 * this->_size_scale * (double)i, pen);
    }

    pen.setColor(QColor(90, 90, 90, 100));
    scene->addLine(-this->_gap * 10.00, 0.00, this->_size_scale + this->_gap * 10.00, 0.00, pen);
    scene->addLine(this->_size_scale, -this->_gap * 10.00, this->_size_scale, this->_size_scale + this->_gap * 10.00, pen);

    pen.setColor(QColor(0, 0, 0, 150));
    scene->addLine(-this->_gap * 10.00, this->_size_scale, this->_size_scale + this->_gap * 10.00, this->_size_scale, pen);
    scene->addLine(0.00, -this->_gap * 10.00, 0.00, this->_size_scale + this->_gap * 10.00, pen);

    QGraphicsTextItem *text_item = scene->addText("0");
    text_item->setScale(this->_text_scale);
    text_item->setPos(2.00 - text_item->boundingRect().width(), this->_size_scale);

    text_item = scene->addText("1");
    text_item->setScale(this->_text_scale);
    text_item->setPos(2.00 + this->_size_scale - text_item->boundingRect().width(), this->_size_scale);

    text_item = scene->addText("1");
    text_item->setScale(this->_text_scale);
    text_item->setPos(2.00 - text_item->boundingRect().width(), 0.00);

    this->_c_1 = scene->addEllipse(-(this->_radius / 2.00), -(this->_radius / 2.00), this->_radius, this->_radius, QPen(QColor(0, 0, 0, 0)), QBrush(QColor(100, 100, 100, 100)));
    this->_c_2 = scene->addEllipse(-(this->_radius / 2.00), -(this->_radius / 2.00), this->_radius, this->_radius, QPen(QColor(0, 0, 0, 0)), QBrush(QColor(100, 100, 100, 100)));

    this->_zero = QPointF(0.00, this->_size_scale);
    this->_one = QPointF(this->_size_scale, 0.00);

    this->_c_1->setPos(0.00, 0.00);
    this->_c_2->setPos(this->_size_scale, this->_size_scale);

    pen.setColor(QColor(255, 122, 0, 128));
    pen.setWidth(this->_secondary_line_width);

    this->_line_0 = scene->addLine(QLineF(this->_zero, this->_c_1->pos()), pen);
    this->_line_1 = scene->addLine(QLineF(this->_c_2->pos(), this->_one), pen);

    pen.setColor(QColor(100, 100, 100, 100));
    this->_line_x = scene->addLine(QLineF(QPointF(0.00, 0.00), QPointF(0.00, 0.00)), pen);

    this->_line_y = scene->addLine(QLineF(QPointF(0.00, 0.00), QPointF(0.00, 0.00)), pen);
    this->_line_x->hide();
    this->_line_y->hide();

    this->_text_x = scene->addText("0");
    this->_text_x->setScale(this->_text_scale);

    this->_text_y = scene->addText("1");
    this->_text_y->setScale(this->_text_scale);

    this->_text_x->hide();
    this->_text_y->hide();

    this->_draw();
    this->_generate();
}

BezierCurveEditWidget::~BezierCurveEditWidget() {}

void BezierCurveEditWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    this->fitInView(-this->_gap, -this->_gap, this->_size_scale + this->_gap, this->_size_scale + this->_gap, Qt::KeepAspectRatio);
}

void BezierCurveEditWidget::_draw()
{
    Q_CHECK_PTR(this->_c_1);
    Q_CHECK_PTR(this->_c_2);

    QGraphicsScene *scene = this->scene();

    for (int i = 0; i < (int)this->_curve.size(); i++)
        scene->removeItem(this->_curve[i]);

    this->_curve.clear();

    QPen pen(QColor(255, 122, 0, 255));
    pen.setWidth(this->_primary_line_width);

    double p = 0.00;

    double x_0 = this->_bezier(p, this->_zero.x(), this->_c_1->pos().x(), this->_c_2->pos().x(), this->_one.x());
    double y_0 = this->_bezier(p, this->_zero.y(), this->_c_1->pos().y(), this->_c_2->pos().y(), this->_one.y());

    for (int i = 1; i < this->_size_scale + 1; i++)
    {
        double r = i / (double)this->_size_scale;
        double x_1 = this->_bezier(r, this->_zero.x(), this->_c_1->pos().x(), this->_c_2->pos().x(), this->_one.x());
        double y_1 = this->_bezier(r, this->_zero.y(), this->_c_1->pos().y(), this->_c_2->pos().y(), this->_one.y());
        this->_curve.push_back(scene->addLine(x_0, y_0, x_1, y_1, pen));
        p = r;
        x_0 = x_1;
        y_0 = y_1;
    }
}

// https://en.wikipedia.org/wiki/B%C3%A9zier_curve
double BezierCurveEditWidget::_bezier(double t, double a, double b, double c, double d)
{
    return pow((1 - t), 3) * a + 3 * pow((1 - t), 2) * t * b + 3 * (1 - t) * pow(t, 2) * c + pow(t, 3) * d;
}

void BezierCurveEditWidget::mousePressEvent(QMouseEvent *event)
{
    Q_CHECK_PTR(this->_c_1);
    Q_CHECK_PTR(this->_c_2);
    Q_CHECK_PTR(this->_line_x);
    Q_CHECK_PTR(this->_line_y);
    Q_CHECK_PTR(this->_text_x);
    Q_CHECK_PTR(this->_text_y);

    this->_prev = event->pos();
    QList<QGraphicsItem *> items = this->items(this->_prev);
    QGraphicsEllipseItem *e;
    for (int i = 0; i < items.count(); i++)
    {
        if ((e = dynamic_cast<QGraphicsEllipseItem *>(items.at(i))))
        {
            this->_line_x->hide();
            this->_line_y->hide();
            this->_text_x->hide();
            this->_text_y->hide();
            if (e == this->_c_1)
            {
                this->_move_c_1 = true;
                return;
            }
            else if (e == this->_c_2)
            {
                this->_move_c_2 = true;
                return;
            }
        }
    }
}

void BezierCurveEditWidget::mouseReleaseEvent(QMouseEvent *event)
{

    Q_CHECK_PTR(this->_c_1);
    Q_CHECK_PTR(this->_c_2);
    Q_CHECK_PTR(this->_line_x);
    Q_CHECK_PTR(this->_line_y);
    Q_CHECK_PTR(this->_text_x);
    Q_CHECK_PTR(this->_text_y);

    this->_line_x->show();
    this->_line_y->show();
    this->_text_x->show();
    this->_text_y->show();

    this->_move_c_1 = false;
    this->_move_c_2 = false;

    this->_generate();
}

void BezierCurveEditWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_CHECK_PTR(this->_c_1);
    Q_CHECK_PTR(this->_c_2);

    Q_CHECK_PTR(this->_line_0);
    Q_CHECK_PTR(this->_line_1);

    Q_CHECK_PTR(this->_line_x);
    Q_CHECK_PTR(this->_line_y);
    Q_CHECK_PTR(this->_text_x);
    Q_CHECK_PTR(this->_text_y);

    QPoint pos = event->pos();
    QPointF c = this->mapToScene(pos);

    if (event->buttons() == Qt::LeftButton)
    {
        QPointF p = this->mapToScene(this->_prev);
        this->_prev = pos;

        if (this->_move_c_1)
        {
            this->_c_1->setPos(this->_c_1->pos() - (p - c));

            if (this->_c_1->pos().x() < 0)
                this->_c_1->setPos(0.00, this->_c_1->pos().y());
            if (this->_c_1->pos().x() > this->_size_scale)
                this->_c_1->setPos(this->_size_scale, this->_c_1->pos().y());

            if (this->_c_1->pos().y() < -this->_gap / 2.00)
                this->_c_1->setPos(this->_c_1->pos().x(), -this->_gap / 2.00);
            if (this->_c_1->pos().y() > this->_size_scale + this->_gap / 2.00)
                this->_c_1->setPos(this->_c_1->pos().x(), this->_size_scale + this->_gap / 2.00);

            this->_line_0->setLine(QLineF(this->_zero, this->_c_1->pos()));
            this->_draw();
            return;
        }
        else if (this->_move_c_2)
        {
            this->_c_2->setPos(this->_c_2->pos() - (p - c));

            if (this->_c_2->pos().x() < 0)
                this->_c_2->setPos(0.00, this->_c_2->pos().y());
            if (this->_c_2->pos().x() > this->_size_scale)
                this->_c_2->setPos(this->_size_scale, this->_c_2->pos().y());

            if (this->_c_2->pos().y() < -this->_gap / 2.00)
                this->_c_2->setPos(this->_c_2->pos().x(), -this->_gap / 2.00);
            if (this->_c_2->pos().y() > this->_size_scale + this->_gap / 2.00)
                this->_c_2->setPos(this->_c_2->pos().x(), this->_size_scale + this->_gap / 2.00);

            this->_line_1->setLine(QLineF(this->_c_2->pos(), this->_one));
            this->_draw();
            return;
        }
    }
    else if (c.x() < 0 || c.x() > this->_size_scale)
    {
        this->_line_x->hide();
        this->_line_y->hide();
        this->_text_x->hide();
        this->_text_y->hide();
    }
    else
    {
        XLoc loc = this->_table[(int)((c.x() / (double)this->_size_scale) * this->_samples)];
        if (!((int)((c.x() / (double)this->_size_scale) * this->_samples) >= 3 && loc.x == 0.0 && loc.y == 0))
        {
            this->_line_x->setLine(loc.x * this->_size_scale, this->_size_scale, loc.x * this->_size_scale, this->_size_scale - loc.y * this->_size_scale);
            this->_line_y->setLine(0.00, this->_size_scale - loc.y * this->_size_scale, loc.x * this->_size_scale, this->_size_scale - loc.y * this->_size_scale);

            this->_text_x->setPlainText(QString().sprintf("%0.2f", loc.x));
            this->_text_x->setPos(loc.x * this->_size_scale - 8.5f, this->_size_scale + 8.00f);

            this->_text_y->setPlainText(QString().sprintf("%0.2f", loc.y));
            this->_text_y->setPos(-25.0f, this->_size_scale - loc.y * this->_size_scale - 5.5f);
        }

        this->_line_x->show();
        this->_line_y->show();
        this->_text_x->show();
        this->_text_y->show();
    }
}

void BezierCurveEditWidget::_generate()
{

    Q_CHECK_PTR(this->_c_1);
    Q_CHECK_PTR(this->_c_2);

    this->_table.clear();
    for (int i = 0; i < this->_samples; i++)
    {
        double v = i / (double)this->_samples;
        double x = this->_bezier(v,
                                 this->_zero.x() / this->_size_scale,
                                 this->_c_1->pos().x() / this->_size_scale,
                                 this->_c_2->pos().x() / this->_size_scale,
                                 this->_one.x() / this->_size_scale);
        double y = 1.00 - this->_bezier(v,
                                        this->_zero.y() / this->_size_scale,
                                        this->_c_1->pos().y() / this->_size_scale,
                                        this->_c_2->pos().y() / this->_size_scale,
                                        this->_one.y() / this->_size_scale);
        this->_table.insert(std::make_pair((int)(x * this->_samples), XLoc{x, y, v}));
    }
    emit this->curveChanged();
}

double BezierCurveEditWidget::f(double x)
{
    if (x < 0.00)
        x = 0.00;
    if (x > 1.00)
        x = 1.00;

    return this->_table[(int)(x * this->_samples)].y;
}