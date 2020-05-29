#include "bezierwidget.h"

#include <math.h> // pow

#include <QTransform>
#include <QDebug>

// Clamps an items position (control or end point) between 0 and 100 in the x coordinates
// between min and max in the x coordinates, and betwen -25 and 125 in the y coordinates
static bool clamp(QGraphicsItem *item, double min, double max)
{
    QPointF c_pos = item->pos();

    // If we applied a clamp
    bool clamped = false;

    if (c_pos.x() < min)
    {
        c_pos.setX(min);
        clamped = true;
    }
    else if (c_pos.x() < 0.00)
    {
        c_pos.setX(0.00);
        clamped = true;
    }
    else if (c_pos.x() > max)
    {
        c_pos.setX(max);
        clamped = true;
    }
    else if (c_pos.x() > 100.00)
    {
        c_pos.setX(100.00);
        clamped = true;
    }

    if (c_pos.y() < -25.00)
    {
        c_pos.setY(-25.00);
        clamped = true;
    }
    else if (c_pos.y() > 125.00)
    {
        c_pos.setY(125.00);
        clamped = true;
    }

    // Apply changes (if any of the above checks pass)
    item->setPos(c_pos);
    return clamped;
}

// https://en.wikipedia.org/wiki/B%C3%A9zier_curve
// Bezier function for a single value
double bezier(double t, double a, double b, double c, double d)
{
    return pow(1 - t, 3) * a + 3 * pow(1 - t, 2) * t * b + 3 * (1 - t) * pow(t, 2) * c + pow(t, 3) * d;
}

// Bezier function for a point
QPointF bezier(double t, QPointF a, QPointF b, QPointF c, QPointF d)
{
    return QPointF(
        bezier(t, a.x(), b.x(), c.x(), d.x()),
        bezier(t, a.y(), b.y(), c.y(), d.y()));
}

// =================================================================
// =================================================================
// =================================================================

// Bezier class for drawing a bezier curve
Bezier::Bezier() {}
Bezier::Bezier(QPen l_pen,
               QPen c_pen,
               QBrush c_brush,
               QPen e_pen,
               QBrush e_brush,
               QPen cu_pen,
               float r,
               float c_thick,
               float l_width,
               float c_width,
               QPointF c_0,
               QPointF c_1,
               QGraphicsEllipseItem *e_0,
               QGraphicsEllipseItem *e_1,
               QGraphicsScene *s)
    : line_pen(l_pen),
      control_pen(c_pen),
      control_brush(c_brush),
      end_pen(e_pen),
      end_brush(e_brush),
      curve_pen(cu_pen),
      radius(r),
      control_thick(c_thick),
      line_width(l_width),
      curve_width(c_width),
      end_0(e_0),
      end_1(e_1),
      scene(s)
{
    // Must have valid pointer for the passed 3 pointers
    Q_CHECK_PTR(this->scene);
    Q_CHECK_PTR(this->end_0);
    Q_CHECK_PTR(this->end_1);

    // Create new control points
    this->control_0 = this->scene->addEllipse(
        0,
        0,
        this->radius, this->radius, this->control_pen, this->control_brush);
    this->control_0->setPos(c_0);
    this->control_0->setTransform(QTransform(1.00, 0.00, 0.00, 1.00, -this->radius / 2.0f, -this->radius / 2.0f));
    this->control_0->setZValue(0.0);

    this->control_1 = this->scene->addEllipse(
        0,
        0,
        this->radius, this->radius, this->control_pen, this->control_brush);
    this->control_1->setPos(c_1);
    this->control_1->setTransform(QTransform(1.00, 0.00, 0.00, 1.00, -this->radius / 2.0f, -this->radius / 2.0f));
    this->control_1->setZValue(0.0);

    // Create control lines
    this->line_0 = this->scene->addLine(
        QLineF(this->end_0->pos(), c_0), this->line_pen);

    this->line_1 = this->scene->addLine(
        QLineF(c_1, this->end_1->pos()), this->line_pen);

    clamp(this->control_0, this->end_0->pos().x(), this->end_1->pos().x());
    clamp(this->control_1, this->end_0->pos().x(), this->end_1->pos().x());

    // Hide editing elements
    this->end_0->hide();
    this->end_1->hide();
    this->control_0->hide();
    this->control_1->hide();
    this->line_0->hide();
    this->line_1->hide();

    qDebug("Created bezier, (%0.2f, %0.2f), (%0.2f, %0.2f), (%0.2f, %0.2f), (%0.2f, %0.2f)",
           this->end_0->pos().x(), this->end_0->pos().y(),
           this->control_0->pos().x(), this->control_0->pos().y(),
           this->control_1->pos().x(), this->control_1->pos().y(),
           this->end_1->pos().x(), this->end_1->pos().y());

    // Draw the curve
    this->draw();
}

// Get the intersection of the line with the bezier curve
// Iterates over the line segments for intersections
bool Bezier::intersect(QLineF line, QPointF *pos)
{
    Q_CHECK_PTR(this->end_0);
    Q_CHECK_PTR(this->end_1);

    double x = line.p1().x();

    // Not within range, return false
    if (x < this->end_0->pos().x() || x > this->end_1->pos().x())
    {
        return false;
    }

    // Find an intersetion point by iterating over each line segment
    for (int i = 0; i < (int)this->curve.size(); i++)
    {
        if (line.intersects(this->curve[i]->line(), pos) == QLineF::BoundedIntersection)
        {
            return true;
        }
    }
    return false;
}

// Draw the bezier curve and update the control lines
void Bezier::draw()
{
    Q_CHECK_PTR(this->end_0);
    Q_CHECK_PTR(this->end_1);
    Q_CHECK_PTR(this->control_0);
    Q_CHECK_PTR(this->control_1);
    Q_CHECK_PTR(this->line_0);
    Q_CHECK_PTR(this->line_1);

    Q_CHECK_PTR(this->scene);

    // The control positions
    QPointF a = this->end_0->pos();
    QPointF b = this->control_0->pos();
    QPointF c = this->control_1->pos();
    QPointF d = this->end_1->pos();

    // The number of samples (the entire widget line segment gets 2048 samples, each bezier gets a percentage of that)
    int samples = (int)((this->end_1->pos().x() - this->end_0->pos().x()) * 2048 / 100);

    // Update control lines
    this->line_0->setLine(QLineF(a, b));
    this->line_1->setLine(QLineF(c, d));

    // Clear the current existing curve curve
    this->clear();

    QPointF prev = a;
    // For each pair of points along the curve using the bezier fuction
    // draw the line segments
    for (int i = 1; i < samples + 1; i++)
    {
        double perc = i / (double)samples;
        QPointF cur = bezier(perc, a, b, c, d);
        this->curve.push_back(this->scene->addLine(QLineF(prev, cur), curve_pen));
        prev = cur;
    }
}

// Clears the current curve
void Bezier::clear()
{
    Q_CHECK_PTR(this->scene);
    // Remove lines from the scene
    for (int i = 0; i < (int)this->curve.size(); i++)
    {
        this->scene->removeItem(this->curve[i]);
    }
    // Empty the vector list
    this->curve.clear();
}

// Remove the elements of the bezier (curve, control points, and lines)
void Bezier::remove(bool keep_ellipses)
{
    Q_CHECK_PTR(this->scene);
    Q_CHECK_PTR(this->end_0);
    Q_CHECK_PTR(this->end_1);
    Q_CHECK_PTR(this->control_0);
    Q_CHECK_PTR(this->control_1);
    Q_CHECK_PTR(this->line_0);
    Q_CHECK_PTR(this->line_1);

    qDebug("Removing bezier, ellipse are kept ? (%s)", keep_ellipses ? "true" : "false");

    this->clear();

    if (!keep_ellipses)
    {
        this->scene->removeItem(this->end_0);
        this->scene->removeItem(this->end_1);
    }
    this->scene->removeItem(this->control_0);
    this->scene->removeItem(this->control_1);
    this->scene->removeItem(this->line_0);
    this->scene->removeItem(this->line_1);
}

// Hide the editing elements
void Bezier::hide()
{
    Q_CHECK_PTR(this->end_0);
    Q_CHECK_PTR(this->end_1);

    Q_CHECK_PTR(this->control_0);
    Q_CHECK_PTR(this->control_1);

    Q_CHECK_PTR(this->line_0);
    Q_CHECK_PTR(this->line_1);

    this->end_0->hide();
    this->end_1->hide();

    this->control_0->hide();
    this->control_1->hide();

    this->line_0->hide();
    this->line_1->hide();
}

// Show the editing elements
void Bezier::show()
{
    Q_CHECK_PTR(this->end_0);
    Q_CHECK_PTR(this->end_1);

    Q_CHECK_PTR(this->control_0);
    Q_CHECK_PTR(this->control_1);

    Q_CHECK_PTR(this->line_0);
    Q_CHECK_PTR(this->line_1);

    this->end_0->show();
    this->end_1->show();

    this->control_0->show();
    this->control_1->show();

    this->line_0->show();
    this->line_1->show();
}

// Split the bezier curve in half
Bezier *Bezier::split()
{
    qDebug("Splitting bezier");
    Q_CHECK_PTR(this->end_0);
    Q_CHECK_PTR(this->end_1);

    Q_CHECK_PTR(this->control_0);
    Q_CHECK_PTR(this->control_1);

    Q_CHECK_PTR(this->line_0);
    Q_CHECK_PTR(this->line_1);

    Q_CHECK_PTR(this->scene);

    // Get the mid point values of the control lines, used for generating new bezier values
    QLineF a(this->end_0->pos(), this->control_0->pos());
    QLineF b(this->end_1->pos(), this->control_1->pos());
    QLineF c(this->control_0->pos(), this->control_1->pos());

    QPointF mid_a = a.pointAt(0.5);
    QPointF mid_b = b.pointAt(0.5);
    QPointF mid_c = c.pointAt(0.5);

    a = QLineF(mid_a, mid_c);
    b = QLineF(mid_b, mid_c);

    QPointF left = a.pointAt(0.5);
    QPointF right = b.pointAt(0.5);

    // The new end point location
    QPointF mid = bezier(0.5, this->end_0->pos(), this->control_0->pos(), this->control_1->pos(), this->end_1->pos());

    // Create new end point and lines
    QGraphicsEllipseItem *left_end_1 = this->scene->addEllipse(0, 0, this->radius, this->radius, this->end_pen, this->end_brush);
    left_end_1->setTransform(QTransform(1.00, 0.00, 0.00, 1.00, -this->radius / 2.0f, -this->radius / 2.0f));
    left_end_1->setPos(mid - QPointF(this->radius / 2.0f, this->radius / 2.0f));

    // Create new beziers
    Bezier left_b(this->line_pen,
                  this->control_pen,
                  this->control_brush,
                  this->end_pen,
                  this->end_brush,
                  this->curve_pen,
                  this->radius,
                  this->control_thick,
                  this->line_width,
                  this->curve_width,
                  this->control_0->pos(),
                  left,
                  this->end_0,
                  left_end_1,
                  this->scene);

    Bezier right_b(this->line_pen,
                   this->control_pen,
                   this->control_brush,
                   this->end_pen,
                   this->end_brush,
                   this->curve_pen,
                   this->radius,
                   this->control_thick,
                   this->line_width,
                   this->curve_width,
                   right,
                   this->control_1->pos(),
                   left_end_1,
                   this->end_1,
                   this->scene);

    left_b.show();
    right_b.show();

    // Remove this bezier, (new ones replace this)
    this->scene->removeItem(this->control_0);
    this->scene->removeItem(this->control_1);
    this->scene->removeItem(this->line_0);
    this->scene->removeItem(this->line_1);

    this->clear();

    // Return the new beziers
    Bezier *ret = new Bezier[2];
    ret[0] = left_b;
    ret[1] = right_b;

    return ret;
}

// =================================================================
// =================================================================
// =================================================================

// Creates bezier classes with the same stylings
// Default stylings
BezierFactory::BezierFactory(QGraphicsScene *s) : scene(s)
{
    this->line_pen.setColor(QColor(104, 160, 255, 100));
    this->line_pen.setWidth(this->line_width);
    this->line_pen.setStyle(Qt::DashLine);
    this->line_pen.setCapStyle(Qt::RoundCap);

    this->curve_pen.setColor(QColor(255, 139, 0, 255));
    this->curve_pen.setWidth(this->curve_width);

    this->control_pen.setColor(QColor(0, 0, 0, 255));
    this->control_pen.setWidth(control_thick);

    this->control_brush.setColor(QColor(100, 100, 100, 150));
    this->control_brush.setStyle(Qt::SolidPattern);

    this->end_pen.setWidth(control_thick);

    this->end_brush.setColor(QColor(199, 155, 87, 150));
    this->end_brush.setStyle(Qt::SolidPattern);
}
// Overwritten stylings
BezierFactory::BezierFactory(QPen l_pen,
                             QPen c_pen,
                             QBrush c_brush,
                             QPen e_pen,
                             QBrush e_brush,
                             QPen cu_pen,
                             float r,
                             float c_thick,
                             float l_width,
                             float c_width,
                             QGraphicsScene *s)
    : line_pen(l_pen),
      control_pen(c_pen),
      control_brush(c_brush),
      end_pen(e_pen),
      end_brush(e_brush),
      curve_pen(cu_pen),
      radius(r),
      control_thick(c_thick),
      line_width(l_width),
      curve_width(c_width),
      scene(s) {}

// Create a new bezier with the local stylings
Bezier BezierFactory::newBezier(QPointF c_0,
                                QPointF c_1,
                                QGraphicsEllipseItem *e_0,
                                QGraphicsEllipseItem *e_1)
{
    Q_CHECK_PTR(this->scene);
    return Bezier(
        this->line_pen,
        this->control_pen,
        this->control_brush,
        this->end_pen,
        this->end_brush,
        this->curve_pen,
        this->radius,
        this->control_thick,
        this->line_width,
        this->curve_width,
        c_0, c_1, e_0, e_1, this->scene);
}

// Create a new ellipse value (for end points shared between beziers)
QGraphicsEllipseItem *BezierFactory::newEllipse(QPointF pos)
{
    Q_CHECK_PTR(this->scene);
    QGraphicsEllipseItem *item = this->scene->addEllipse(0,
                                                         0,
                                                         this->radius, this->radius,
                                                         this->end_pen, this->end_brush);
    item->setPos(pos);
    item->setTransform(QTransform(1.00, 0.00, 0.00, 1.00, -this->radius / 2.0f, -this->radius / 2.0f));
    item->setZValue(1.0);

    return item;
}

// =================================================================
// =================================================================
// =================================================================

// Bezier editting widget
BezierEditWidget::BezierEditWidget(int divs, QWidget *parent) : QGraphicsView(parent)
{
    qDebug("Creating new Bezier widget");
    // Create the scene and viewport
    QGraphicsScene *scene = new QGraphicsScene();
    this->setScene(scene);
    this->scale(1.00, -1.00); // Flip for y+ is up, not down
    this->setSceneRect(0.00, 0.00, 100.00, 100.00);
    this->fitInView(-50.00, -50.00, 150.00, 150.00, Qt::KeepAspectRatio);

    // New bezier factory to have all segments the same styling
    this->_factory = new BezierFactory(scene);

    // Background pen
    QPen pen(QColor(0, 0, 0, 255));
    pen.setWidth(0.5f);

    // X and Y = 0 lines
    scene->addLine(-1000.00, 0.00, 1000.00, 0.00, pen);
    scene->addLine(0.00, -1000.00, 0.00, 1000.00, pen);

    pen.setColor(QColor(100, 100, 100, 100));

    // X and Y = 100 lines
    scene->addLine(100.00, 0.00, 100.00, 100.00, pen);
    scene->addLine(0.00, 100.00, 100.00, 100.00, pen);

    pen.setColor(QColor(100, 100, 100, 50));

    // Line segments between 0 and 100 (x and y)
    for (int i = 1; i < divs; i++)
    {
        double off = 1.0 / (double)divs * (double)i * 100.00;
        scene->addLine(off, 0.0, off, 100.00, pen);
        scene->addLine(0.00, off, 100.00, off, pen);
    }

    // Y = X line
    pen.setStyle(Qt::DashLine);
    scene->addLine(0.00, 0.00, 100.00, 100.00, pen);

    // Text values
    QGraphicsTextItem *text = scene->addText("0"); // x = y = 0
    text->setPos(-14.00, 2.00);
    text->setTransform(QTransform(1.00, 0.00, 0.00, -1.00, 0.00, 0.00)); // flip along y to counter entire scene flip

    text = scene->addText("1"); // x = 1
    text->setPos(-14.00, 112.00);
    text->setTransform(QTransform(1.00, 0.00, 0.00, -1.00, 0.00, 0.00)); // flip along y to counter entire scene flip

    text = scene->addText("1"); // y = 1
    text->setPos(95.00, 2.00);
    text->setTransform(QTransform(1.00, 0.00, 0.00, -1.00, 0.00, 0.00)); // flip along y to counter entire scene flip

    // Default first curve
    QGraphicsEllipseItem *e_0 = this->_factory->newEllipse(QPointF(0.0f, 0.0f));
    QGraphicsEllipseItem *e_1 = this->_factory->newEllipse(QPointF(100.0f, 100.0f));

    this->_beziers.push_back(this->_factory->newBezier(QPointF(50.0f, 0.0f), QPointF(50.0f, 100.0f), e_0, e_1));

    // Overdraw lines for 0 to min of first bezier, and max of last bezier to 100
    this->_line_start = scene->addLine(0.00, 0.00, 0.00, 0.00, this->_factory->curve_pen);
    this->_line_end = scene->addLine(100.00, 100.00, 100.00, 100.00, this->_factory->curve_pen);

    pen.setColor(QColor(0, 0, 0, 100));
    pen.setStyle(Qt::DashLine);

    // Preview lines
    this->_line_x = scene->addLine(0.00, 0.00, 0.00, 100.00, pen);
    this->_line_y = scene->addLine(0.00, 0.00, 100.00, 0.00, pen);

    // Preview values
    this->_text_x = scene->addText("0.00");
    this->_text_x->setScale(0.75);
    this->_text_x->setPos(0.00, 117.00);
    this->_text_x->setTransform(QTransform(1.00, 0.00, 0.00, -1.00, 0.00, 0.00));

    this->_text_y = scene->addText("0.00");
    this->_text_y->setScale(0.75);
    this->_text_y->setPos(100.00, 10.00);
    this->_text_y->setTransform(QTransform(1.00, 0.00, 0.00, -1.00, 0.00, 0.00));

    // Hide editing elements
    this->_line_x->hide();
    this->_line_y->hide();
    this->_text_x->hide();
    this->_text_y->hide();

    for (int i = 0; i < (int)this->_beziers.size(); i++)
        this->_beziers[i].hide();
}

// Get the intersection value for a line
bool BezierEditWidget::intersect(QLineF line, QPointF *pos)
{
    Q_CHECK_PTR(this->_line_start);
    Q_CHECK_PTR(this->_line_end);
    Q_CHECK_PTR(this->_beziers[0].end_0);
    Q_CHECK_PTR(this->_beziers[(int)this->_beziers.size() - 1].end_1);

    // Values for checks
    double x = line.p1().x();
    double b_min = this->_beziers[0].end_0->pos().x();
    double b_max = this->_beziers[(int)this->_beziers.size() - 1].end_1->pos().x();

    // If below the first bezier, use the left overdraw line
    if (this->_line_start->line().intersects(line, pos) && x < b_min)
        return true;

    // Find intersection from the different beziers
    for (int i = 0; i < (int)this->_beziers.size(); i++)
        if (this->_beziers[i].intersect(line, pos))
            return true;

    // If above the last bezier, use the right overdraw line
    if (this->_line_end->line().intersects(line, pos) && x > b_max)
        return true;

    return false;
}

// Get the f(x) value, value is [0, 1] and returns [0, 1], hard clamps to [0, 1]
double BezierEditWidget::valueAt(double value)
{
    if (value < 0.00)
        value = 0.00;
    if (value > 1.00)
        value = 1.00;

    QLineF line(value * 100.00, -50.00, value * 100.00, 150.00);

    QPointF pos;
    if (this->intersect(line, &pos))
    {
        return (double)(pos.y() / 100.00);
    }
    return 0.00;
}

// Save the bezier curves as a series of points
std::vector<QPointF> BezierEditWidget::save()
{
    qDebug("Saving curve data");
    std::vector<QPointF> values;
    for (int i = 0; i < (int)this->_beziers.size(); i++)
    {
        Q_CHECK_PTR(this->_beziers[i].end_0);
        Q_CHECK_PTR(this->_beziers[i].control_0);
        Q_CHECK_PTR(this->_beziers[i].control_1);
        Q_CHECK_PTR(this->_beziers[i].end_1);
        if (i == 0)
            values.push_back(this->_beziers[i].end_0->pos());
        values.push_back(this->_beziers[i].control_0->pos());
        values.push_back(this->_beziers[i].control_1->pos());
        values.push_back(this->_beziers[i].end_1->pos());
    }
    return values;
}

// Restore the bezier curve from a series of points
void BezierEditWidget::restore(std::vector<QPointF> values)
{
    qDebug("Restoring curve data");
    Q_CHECK_PTR(this->_factory);
    Q_CHECK_PTR(this->_line_start);
    Q_CHECK_PTR(this->_line_end);
    if (values.size() < 4)
        return;

    this->_clear();

    // Create first bezier (from first 4 points)
    QGraphicsEllipseItem *end_0 = this->_factory->newEllipse(values.front());
    values.erase(values.begin());

    QPointF control_0 = values.front();
    values.erase(values.begin());

    QPointF control_1 = values.front();
    values.erase(values.begin());

    QGraphicsEllipseItem *end_1 = this->_factory->newEllipse(values.front());
    values.erase(values.begin());

    this->_beziers.push_back(this->_factory->newBezier(control_0, control_1, end_0, end_1));
    end_0 = end_1;

    // Create rest of the curves from the tuples of 3 points (and previous curve end point)
    while (values.size() > 2)
    {
        control_0 = values.front();
        values.erase(values.begin());

        control_1 = values.front();
        values.erase(values.begin());

        end_1 = this->_factory->newEllipse(values.front());
        values.erase(values.begin());

        this->_beziers.push_back(this->_factory->newBezier(control_0, control_1, end_0, end_1));

        end_0 = end_1;
    }

    // Fix overdraw lines
    double x = this->_beziers[0].end_0->pos().x();
    double y = this->_beziers[0].end_0->pos().y();

    this->_line_start->setLine(QLineF(0.00, y, x, y));

    x = this->_beziers[(int)this->_beziers.size() - 1].end_1->pos().x();
    y = this->_beziers[(int)this->_beziers.size() - 1].end_1->pos().y();

    this->_line_end->setLine(QLineF(x, y, 100.00, y));
}

// Removes the curves (used for restoring)
void BezierEditWidget::_clear()
{
    for (int i = 0; i < (int)this->_beziers.size(); i++)
        this->_beziers[i].remove();

    this->_beziers.clear();
}

// On resize update viewport
void BezierEditWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    this->fitInView(-50.00, -50.00, 150.00, 150.00, Qt::KeepAspectRatio);
}

// On move, update preview values, move control points (if clicked)
void BezierEditWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_CHECK_PTR(this->_line_x);
    Q_CHECK_PTR(this->_line_y);

    Q_CHECK_PTR(this->_text_x);
    Q_CHECK_PTR(this->_text_y);

    Q_CHECK_PTR(this->_line_start);
    Q_CHECK_PTR(this->_line_end);

    // Get the position
    QPointF pos = this->mapToScene(event->pos());

    // If within range, draw the preview lines and values
    if (-2.00 <= pos.x() && pos.x() <= 102.00)
    {
        if (pos.x() < 0.00)
            pos.setX(0.00);
        if (pos.x() > 100.00)
            pos.setX(100.00);
        this->_line_x->setLine(QLineF(pos.x(), 0.00, pos.x(), 100.00));
        this->_text_x->setPos(pos.x() - 3.00, 117.0);
    }

    // The x intersection line
    QLineF line = this->_line_x->line();
    line.setP1(QPointF(line.x1(), -50.0f));
    line.setP2(QPointF(line.x1(), 150.0f));

    // Get the intersection point
    // and draw the y values, show values
    if (this->intersect(line, &pos))
    {
        this->_text_x->show();
        this->_text_x->setPlainText(QString::asprintf("%0.2f", pos.x() / 100.0));

        this->_text_y->show();
        this->_text_y->setPlainText(QString::asprintf("%0.2f", pos.y() / 100.0));
        this->_text_y->setPos(100.00, pos.y() + 13.00);

        this->_line_y->setLine(QLineF(0.00, pos.y(), 100.00, pos.y()));
    }
    // Hide text values if outside of preview range
    else
    {
        this->_text_x->hide();
        this->_text_y->hide();
    }

    // If we are clicking (i.e. assumed editing control points, hide preview values)
    if (event->buttons() == Qt::LeftButton)
    {
        this->_text_x->hide();
        this->_text_y->hide();
        this->_line_x->hide();
        this->_line_y->hide();
    }

    // Get the position in the scene
    pos = this->mapToScene(event->pos());

    // Get the relative movement
    QPointF diff = pos - this->_prev;

    // Update previous relative position
    this->_prev = pos;

    // Clamping values
    double min, max;

    // We are moving a control point
    if (this->_control)
    {
        // Move the control point
        QPointF prev_pos = this->_control->pos();
        this->_control->setPos(this->_control->pos() + diff);

        // Clamp control point, default values
        min = 0.00;
        max = 100.00;
        // Clamping checks (if only one bezier, we are clamping a control point
        // between a specific bezier curve, if two beziers, we are claming an end point
        // and its matching control points)
        if (this->_bezier_0 && !this->_bezier_1)
        {
            min = this->_bezier_0->end_0->x();
            max = this->_bezier_0->end_1->x();
        }
        else if (!this->_bezier_0 && this->_bezier_1)
        {
            min = this->_bezier_1->end_0->x();
            max = this->_bezier_1->end_1->x();
        }
        else if (this->_bezier_0 && this->_bezier_1)
        {
            min = this->_bezier_0->end_0->x();
            max = this->_bezier_1->end_1->x();
        }

        // We clamped the control point, the relative movement is now different
        if (clamp(this->_control, min, max))
            diff = this->_control->pos() - prev_pos;

        // We are also moving other control points, move them as well
        if (this->_extra_0)
            this->_extra_0->setPos(this->_extra_0->pos() + diff);

        if (this->_extra_1)
            this->_extra_1->setPos(this->_extra_1->pos() + diff);

        // Clamp the extra values as well
        min = 0.00;
        max = 100.00;
        if (this->_bezier_0)
        {
            min = this->_bezier_0->end_0->x();
            max = this->_bezier_0->end_1->x();
            clamp(this->_bezier_0->control_0, min, max);
            clamp(this->_bezier_0->control_1, min, max);
        }

        min = 0.00;
        max = 100.00;
        if (this->_bezier_1)
        {
            min = this->_bezier_1->end_0->x();
            max = this->_bezier_1->end_1->x();
            clamp(this->_bezier_1->control_0, min, max);
            clamp(this->_bezier_1->control_1, min, max);
        }

        // Update the bezier curves
        if (this->_bezier_0)
            this->_bezier_0->draw();

        if (this->_bezier_1)
            this->_bezier_1->draw();

        // Update the overdraw lines
        QPointF start = this->_beziers[0].end_0->pos();
        QPointF end = this->_beziers[(int)this->_beziers.size() - 1].end_1->pos();

        this->_line_start->setLine(QLineF(0.00, start.y(), start.x(), start.y()));
        this->_line_end->setLine(QLineF(end.x(), end.y(), 100.00, end.y()));
    }
}

// On mouse press, select control points for moving, or collapse a control point to
// its end point, or shift to move a control point out of an end point
void BezierEditWidget::mousePressEvent(QMouseEvent *event)
{
    Q_CHECK_PTR(this->_line_x);
    Q_CHECK_PTR(this->_line_y);

    Q_CHECK_PTR(this->_text_x);
    Q_CHECK_PTR(this->_text_y);
    // Only accept left clicks
    if (event->buttons() == Qt::LeftButton)
    {
        // Click hide the preview lines
        this->_text_x->hide();
        this->_text_y->hide();
        this->_line_x->hide();
        this->_line_y->hide();

        // Get the position in scene
        this->_prev = this->mapToScene(event->pos());

        // Get items at the click point
        QList<QGraphicsItem *> items = this->items(event->pos());
        for (int i = 0; i < items.count(); i++)
        {
            // Filter for ellipse items (only and uniquely editable elements)
            QGraphicsEllipseItem *e;
            if ((e = dynamic_cast<QGraphicsEllipseItem *>(items.at(i))))
            {
                // If pressing shift, we keep looping to find a control, not end point,
                // thus need to confirm we found a point, rather than break, breaking loops
                bool found = false;
                // Find the containing beziers
                for (int j = 0; j < (int)this->_beziers.size(); j++)
                {
                    Bezier *b = &this->_beziers[j];

                    // If we are control clicking and its a control point, collapse the control point to its
                    // coresponding end point
                    if (event->modifiers() == Qt::ControlModifier && (b->control_0 == e || b->control_1 == e))
                    {
                        if (b->control_0 == e)
                            e->setPos(b->end_0->pos());

                        if (b->control_1 == e)
                            e->setPos(b->end_1->pos());

                        b->draw();
                        emit this->curveChanged();
                        return;
                    }

                    // If its an end point, and we are not shift clicking (ignore endpoints)
                    // Setup controls to be affected
                    if (b->end_0 == e && !event->modifiers().testFlag(Qt::ShiftModifier))
                    {
                        this->_control = e;
                        this->_extra_1 = b->control_0;
                        this->_bezier_1 = b;
                        found = true;
                        break;
                    }
                    else if (b->end_1 == e && !event->modifiers().testFlag(Qt::ShiftModifier))
                    {
                        this->_control = e;
                        this->_extra_0 = b->control_1;
                        this->_bezier_0 = b;
                        found = true;
                    }
                    // If its a control point, setup controls to be affected
                    else if (b->control_0 == e || b->control_1 == e)
                    {
                        this->_control = e;
                        this->_bezier_0 = b;
                        found = true;
                        break;
                    }
                }
                if (found)
                    break;
            }
        }
    }
}

// On release, remove control pointers
void BezierEditWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    Q_CHECK_PTR(this->_line_x);
    Q_CHECK_PTR(this->_line_y);

    this->_control = nullptr;
    this->_extra_0 = nullptr;
    this->_extra_1 = nullptr;
    this->_bezier_0 = nullptr;
    this->_bezier_1 = nullptr;

    // Reshow preview lines
    this->_line_x->show();
    this->_line_y->show();

    emit this->curveChanged();
}

// On enter, show control and preview elements
void BezierEditWidget::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    Q_CHECK_PTR(this->_line_x);
    Q_CHECK_PTR(this->_line_y);
    Q_CHECK_PTR(this->_text_x);
    Q_CHECK_PTR(this->_text_y);

    this->_line_x->show();
    this->_line_y->show();
    this->_text_x->show();
    this->_text_y->show();

    for (int i = 0; i < (int)this->_beziers.size(); i++)
        this->_beziers[i].show();
}

// On leave, hide control and preview elements
void BezierEditWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    Q_CHECK_PTR(this->_line_x);
    Q_CHECK_PTR(this->_line_y);
    Q_CHECK_PTR(this->_text_x);
    Q_CHECK_PTR(this->_text_y);

    this->_line_x->hide();
    this->_line_y->hide();
    this->_text_x->hide();
    this->_text_y->hide();

    for (int i = 0; i < (int)this->_beziers.size(); i++)
        this->_beziers[i].hide();
}

// On delete, remove extra beziers (1 min)
void BezierEditWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() != Qt::Key_X)
        return;

    double x = this->_prev.x();

    if (x < 0.00 || x > 100.00)
        return;

    if (this->_beziers.size() <= 1)
        return;

    for (int i = 0; i < (int)this->_beziers.size(); i++)
    {
        Q_CHECK_PTR(this->_beziers[i].end_0);
        Q_CHECK_PTR(this->_beziers[i].end_1);

        double min = this->_beziers[i].end_0->pos().x();
        double max = this->_beziers[i].end_1->pos().x();

        if (min <= x && x <= max)
        {
            this->_beziers[i].remove(true);
            if (i == 0)
            {
                this->scene()->removeItem(this->_beziers[i].end_0);
                this->_beziers.erase(this->_beziers.begin());
            }
            else if (i == (int)this->_beziers.size() - 1)
            {
                this->scene()->removeItem(this->_beziers[i].end_1);
                this->_beziers.erase(this->_beziers.end());
            }
            else
            {
                this->_beziers[i - 1].end_1 = this->_beziers[i + 1].end_0;
                this->_beziers[i - 1].draw();
                this->_beziers[i + 1].draw();
                this->_beziers.erase(this->_beziers.begin() + i);
            }
            // Update the overdraw lines
            QPointF start = this->_beziers[0].end_0->pos();
            QPointF end = this->_beziers[(int)this->_beziers.size() - 1].end_1->pos();

            this->_line_start->setLine(QLineF(0.00, start.y(), start.x(), start.y()));
            this->_line_end->setLine(QLineF(end.x(), end.y(), 100.00, end.y()));
        }
    }
}

// On double click find bezier double clicked and split
void BezierEditWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPointF pos = this->mapToScene(event->pos());
    // Find bezier in the x range
    for (int i = 0; i < (int)this->_beziers.size(); i++)
    {
        // Split the bezier into two
        if (this->_beziers[i].end_0->pos().x() <= pos.x() && pos.x() < this->_beziers[i].end_1->pos().x())
        {
            Bezier *beziers = this->_beziers[i].split();
            this->_beziers[i] = beziers[0];
            this->_beziers.insert(this->_beziers.begin() + i + 1, beziers[1]);
            emit this->curveChanged();
            return;
        }
    }
}