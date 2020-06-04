#include "bezierwidget.h"

#include <math.h>

#include <QDebug>
#include <QTransform>

/******************************************************************************
 *                                HELPERS                                     *
 ******************************************************************************/

/**
 * clamp
 * 
 * Given an item (control or end point) we apply a clamping function on the
 * x-axis ensuring that the object stays within certain bounds. There is also a
 * y-axis bounds to ensure the control points stay within the view of the
 * viewport.
 * 
 * @param QGraphicsItem* item : The item to clamp.
 * @param double min : The minimum x value.
 * @param double max : The maximum x value.
 * 
 * @return bool : Wether or not the clamp was applied.
 */
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

/**
 * bezier
 * 
 * The raw bezier function for a single dimension
 * https://en.wikipedia.org/wiki/B%C3%A9zier_curve
 * 
 * @param double t : The percentage of the bezier to get.
 * @param double a : The first end point.
 * @param double b : The first control point.
 * @param double c : The second control point.
 * @param double d : The second end point.
 * 
 * @returns double : The resulting value.
 */
double bezier(double t, double a, double b, double c, double d)
{
    return pow(1 - t, 3) * a
           + 3 * pow(1 - t, 2) * t * b
           + 3 * (1 - t) * pow(t, 2) * c
           + pow(t, 3) * d;
}

/**
 * bezier
 * 
 * The raw bezier function for a single dimension
 * https://en.wikipedia.org/wiki/B%C3%A9zier_curve
 * 
 * @param double t : The percentage of the bezier to get.
 * @param QPointF a : The first end point.
 * @param QPointF b : The first control point.
 * @param QPointF c : The second control point.
 * @param QPointF d : The second end point.
 * 
 * @returns QPointF : The resulting value.
 */
QPointF bezier(double t, QPointF a, QPointF b, QPointF c, QPointF d)
{
    return QPointF(
        bezier(t, a.x(), b.x(), c.x(), d.x()),
        bezier(t, a.y(), b.y(), c.y(), d.y()));
}

/******************************************************************************
 *                                BEZIER                                      *
 ******************************************************************************/

/**
 * Bezier
 * 
 * Creates a new bezier curve with all the default values used.
 */
Bezier::Bezier() {}

/**
 * Bezier
 * 
 * Creates a new bezier with the overriden values.
 * 
 * @param QPen l_pen : The line pen (between control and end point).
 * @param QPen c_pen : The control pen (outline on a control circle).
 * @param QBrush c_brush : The control brush (fill).
 * @param QPen e_pen : The end point pen (outline).
 * @param QBrush e_brush : The end point brush (fill).
 * @param QPen cu_pen : The curve pen (the bezier curve itself)
 * @param float r : The radius of the control and end point circles.
 * @param float c_thick : The control and end point outline thickness.
 * @param float l_width : The width of the lines between control and end point.
 * @param float c_width : The width of the lines that make up the curve.
 * @param QPointF c_0 : The first control point location.
 * @param QPointF c_1 : The second control point location.
 * @param QGraphicsEllipseItem* e_0 : The pointer to the firstend point (end
 *                                    points are shared between beziers)
 * @param QGraphicsEllipseItem* e_1 : The pointer to the second end point.
 * @param QGraphicsScene* s : The scene that draws the elements
 */
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
    this->control_0->setTransform(QTransform(1.00,
                                             0.00,
                                             0.00,
                                             1.00,
                                             -this->radius / 2.0f,
                                             -this->radius / 2.0f));
    this->control_0->setZValue(0.0);

    this->control_1 = this->scene->addEllipse(
        0,
        0,
        this->radius, this->radius, this->control_pen, this->control_brush);
    this->control_1->setPos(c_1);
    this->control_1->setTransform(QTransform(1.00,
                                             0.00,
                                             0.00,
                                             1.00,
                                             -this->radius / 2.0f,
                                             -this->radius / 2.0f));
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

    qDebug("Created bezier"
           ", (%0.2f, %0.2f), (%0.2f, %0.2f), (%0.2f, %0.2f), (%0.2f, %0.2f)",
           this->end_0->pos().x(), this->end_0->pos().y(),
           this->control_0->pos().x(), this->control_0->pos().y(),
           this->control_1->pos().x(), this->control_1->pos().y(),
           this->end_1->pos().x(), this->end_1->pos().y());

    // Draw the curve
    this->draw();
}

/**
 * intersect
 * 
 * Get the intersection point of a line through the bezier curve. If the line
 * does not intersect with the curve false is return and pos is unchanged.
 * 
 * @param QLineF line : The line to try to intersect with.
 * @param QPointF* pos : The position where the intersection could happen.
 * 
 * @returns bool : Whether the line intersects or not.
 */
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
        if (line.intersects(this->curve[i]->line(), pos)
            == QLineF::BoundedIntersection)
        {
            return true;
        }
    }
    return false;
}

/**
 * draw
 * 
 * Draws the curve using the bezier function to draw segments of line to the
 * scene.
 */
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

    // The number of samples (the entire widget line segment gets 2048 samples,
    // each bezier gets a percentage of that)
    int samples = (int)((this->end_1->pos().x()
                         - this->end_0->pos().x()) * 2048 / 100);

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
        this->curve.push_back(
            this->scene->addLine(QLineF(prev, cur), curve_pen));

        prev = cur;
    }
}

/**
 * clear
 * 
 * Clears the drawn lines from the scene, usually used for redrawing the updated
 * curve.
 */
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

/**
 * remove
 * 
 * Removes the control elements from the scene (end points and ellipses). If the
 * inputted flag is set to true the end points are not removed (used for 
 * splitting a curve).
 * 
 * @param bool keep_ellipses : Whether or not to keep the end points.
 */
void Bezier::remove(bool keep_ellipses)
{
    Q_CHECK_PTR(this->scene);
    Q_CHECK_PTR(this->end_0);
    Q_CHECK_PTR(this->end_1);
    Q_CHECK_PTR(this->control_0);
    Q_CHECK_PTR(this->control_1);
    Q_CHECK_PTR(this->line_0);
    Q_CHECK_PTR(this->line_1);

    qDebug("Removing bezier, ellipse are kept ? (%s)", keep_ellipses ? "true" 
                                                                     : "false");

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

/**
 * hide
 * 
 * Hides the control elements from the scene.
 */
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

/**
 * show
 * 
 * Shows the control elements in the scene.
 */
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

/**
 * split
 * 
 * Splits a bezier curve into two smaller connected bezier curves. It does not
 * accurately match the original.
 * 
 * @returns Bezier* : A list of two bezier curves [Bezier, Bezier].
 * 
 * TODO: Make the split two bezier curves accurately represent the original
 * curve.
 */
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

    // Get the mid point values of the control lines, used for generating new
    // bezier values
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
    QPointF mid = bezier(0.5,
                         this->end_0->pos(),
                         this->control_0->pos(),
                         this->control_1->pos(),
                         this->end_1->pos());

    // Create new end point and lines
    QGraphicsEllipseItem *left_end_1 = this->scene->addEllipse(0,
                                                               0,
                                                               this->radius,
                                                               this->radius,
                                                               this->end_pen,
                                                               this->end_brush);

    left_end_1->setTransform(QTransform(1.00,
                                        0.00,
                                        0.00,
                                        1.00,
                                        -this->radius / 2.0f,
                                        -this->radius / 2.0f));

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

/******************************************************************************
 *                             BEZIER FACTORY                                 *
 ******************************************************************************/

/**
 * BezierFactory
 * 
 * Creates the default factory with the default values.
 * 
 * @param QGraphicsScene* s : The reference scene for curves to draw on.
 */
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

/**
 * BezierFactory
 * 
 * Creates a new bezier factory with the overriden values.
 * 
 * @param QPen l_pen : The line pen (between control and end point).
 * @param QPen c_pen : The control pen (outline on a control circle).
 * @param QBrush c_brush : The control brush (fill).
 * @param QPen e_pen : The end point pen (outline).
 * @param QBrush e_brush : The end point brush (fill).
 * @param QPen cu_pen : The curve pen (the bezier curve itself)
 * @param float r : The radius of the control and end point circles.
 * @param float c_thick : The control and end point outline thickness.
 * @param float l_width : The width of the lines between control and end point.
 * @param float c_width : The width of the lines that make up the curve.
 * @param QGraphicsScene* s : The scene that draws the elements
 */
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

/**
 * newBezier
 * 
 * Creates a new bezier with the internal stylings of the factory.
 * 
 * @param QPointF c_0 : The first control point.
 * @param QPointF c_1 : The second control point.
 * @param QGraphicsEllipseItem* e_0 : The pointer to the first end point.
 * @param QGraphicsEllipseItem* e_1 : The pointer to the second end point.
 * 
 * @returns Bezier : The newly created bezier.
 */
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

/**
 * newEllipse
 * 
 * From a point the new ellipse is created using internal values.
 * 
 * @param QPointF pos : The position of the new ellipse.
 * 
 * @returns QGraphicsEllipseItem* : The pointer to the new ellipse item.
 */
QGraphicsEllipseItem *BezierFactory::newEllipse(QPointF pos)
{
    Q_CHECK_PTR(this->scene);
    QGraphicsEllipseItem *item = this->scene->addEllipse(0,
                                                         0,
                                                         this->radius,
                                                         this->radius,
                                                         this->end_pen,
                                                         this->end_brush);
    item->setPos(pos);
    item->setTransform(QTransform(1.00,
                                  0.00,
                                  0.00,
                                  1.00,
                                  -this->radius / 2.0f,
                                  -this->radius / 2.0f));
    item->setZValue(1.0);

    return item;
}

/******************************************************************************
 *                          BEZIER EDIT WIDGET                                *
 ******************************************************************************/

/**
 * BezierEditWidget
 * 
 * Creates the new bezier editing widget. Sets up the scene and view and creates
 * a basic curve.
 * 
 * @param int divs : The number of section to display for the backround grid.
 *                   Default is 4 (a 4x4 16 squares grid)
 * @param QWidget* parent : The parent housing widget.
 */
BezierEditWidget::BezierEditWidget(int divs, QWidget *parent)
    : QGraphicsView(parent)
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
    // flip along y to counter entire scene flip
    text->setTransform(QTransform(1.00, 0.00, 0.00, -1.00, 0.00, 0.00));

    text = scene->addText("1"); // x = 1
    text->setPos(-14.00, 112.00);
    // flip along y to counter entire scene flip
    text->setTransform(QTransform(1.00, 0.00, 0.00, -1.00, 0.00, 0.00));

    text = scene->addText("1"); // y = 1
    text->setPos(95.00, 2.00);
    // flip along y to counter entire scene flip
    text->setTransform(QTransform(1.00, 0.00, 0.00, -1.00, 0.00, 0.00));

    // Default first curve
    QGraphicsEllipseItem *e_0 = this->_factory->newEllipse(QPointF(0.0f, 0.0f));
    QGraphicsEllipseItem *e_1 =
        this->_factory->newEllipse(QPointF(100.0f, 100.0f));

    this->_beziers.push_back(
        this->_factory->newBezier(
            QPointF(50.0f, 0.0f), QPointF(50.0f, 100.0f), e_0, e_1));

    // Overdraw lines for 0 to min of first bezier, and max of last bezier to 100
    this->_line_start = scene->addLine(0.00,
                                       0.00,
                                       0.00,
                                       0.00,
                                       this->_factory->curve_pen);

    this->_line_end = scene->addLine(100.00,
                                     100.00,
                                     100.00,
                                     100.00,
                                     this->_factory->curve_pen);

    pen.setColor(QColor(0, 0, 0, 100));
    pen.setStyle(Qt::DashLine);

    // Preview lines
    this->_line_x = scene->addLine(0.00, 0.00, 0.00, 100.00, pen);
    this->_line_y = scene->addLine(0.00, 0.00, 100.00, 0.00, pen);

    // Preview values
    this->_text_x = scene->addText("0.00");
    this->_text_x->setScale(0.75);
    this->_text_x->setPos(0.00, 117.00);
    this->_text_x->setTransform(QTransform(1.00,
                                           0.00,
                                           0.00,
                                           -1.00,
                                           0.00,
                                           0.00));

    this->_text_y = scene->addText("0.00");
    this->_text_y->setScale(0.75);
    this->_text_y->setPos(100.00, 10.00);
    this->_text_y->setTransform(QTransform(1.00,
                                           0.00,
                                           0.00,
                                           -1.00,
                                           0.00,
                                           0.00));

    // Hide editing elements
    this->_line_x->hide();
    this->_line_y->hide();
    this->_text_x->hide();
    this->_text_y->hide();

    for (int i = 0; i < (int)this->_beziers.size(); i++)
        this->_beziers[i].hide();
}

/**
 * intersect
 * 
 * Get the intersection of a line with the entire combined bezier curve. Returns
 * true if the intersection was successful. pos is unchanged if the intersect
 * fails.
 * 
 * @param QLineF line : The line to try and intersect with.
 * @param QPointF* pos : The position of the intersection.
 * 
 * @returns bool : Whether or not the intersection was successful.
 */
bool BezierEditWidget::intersect(QLineF line, QPointF *pos)
{
    Q_CHECK_PTR(this->_line_start);
    Q_CHECK_PTR(this->_line_end);
    Q_CHECK_PTR(this->_beziers[0].end_0);
    Q_CHECK_PTR(this->_beziers[(int)this->_beziers.size() - 1].end_1);

    // Values for checks
    double x = line.p1().x();
    double b_min = this->_beziers[0].end_0->pos().x();
    double b_max =
        this->_beziers[(int)this->_beziers.size() - 1].end_1->pos().x();

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

/**
 * valueAt
 * 
 * Get the resutling value as a function of y = f(x). X is clamped between 0 and
 * 1. The value returned is (soft) expected to be between 0 and 1. Since the
 * control points can go above and below y bounds of 0 and 1 the resulting curve
 * can go beyond those bounds thus an f(x) can produce y values less than 0 or
 * greater than 1 as well as between.
 * 
 * @param double value : The value to transform.
 * 
 * @returns double : The transformed value.
 */
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

/**
 * save
 * 
 * Saves the points of the bezier curve in a std::vector for storage or for use
 * in syncing multilple curves.
 * 
 * @returns std::vector<QPointF> : A list of points that make up the curve.
 */
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

/**
 * restore
 * 
 * Restores the state of the widget with a list of given values that make up
 * the points of the curves.
 * 
 * @param std::vector<QPointF> values : The values of the curve.
 */
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

    this->_beziers.push_back(this->_factory->newBezier(control_0,
                                                       control_1,
                                                       end_0,
                                                       end_1));
    end_0 = end_1;

    // Create rest of the curves from the tuples of 3 points (and previous curve
    // end point)
    while (values.size() > 2)
    {
        control_0 = values.front();
        values.erase(values.begin());

        control_1 = values.front();
        values.erase(values.begin());

        end_1 = this->_factory->newEllipse(values.front());
        values.erase(values.begin());

        this->_beziers.push_back(this->_factory->newBezier(control_0,
                                                           control_1,
                                                           end_0,
                                                           end_1));

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

/**
 * _clear
 * 
 * Clears all the bezier curves (for restore).
 */
void BezierEditWidget::_clear()
{
    for (int i = 0; i < (int)this->_beziers.size(); i++)
        this->_beziers[i].remove();

    this->_beziers.clear();
}

/**
 * resizeEvent
 * 
 * Resizes the view to fit the widget in the view keeping aspect ratio.
 * 
 * @param QResizeEvent* event : Unused parameter.
 */
void BezierEditWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    this->fitInView(-50.00, -50.00, 150.00, 150.00, Qt::KeepAspectRatio);
}

/**
 * mouseMoveEvent
 * 
 * When the mouse is moved, so long as a control element is selected it will be
 * dragged and updating the widget. If the user is not manipulating a control
 * element the widget draws a preview x and y axis showing what the y = f(x)
 * would be along the curve.
 * 
 * @param QMouseEvent *event : The mouse move event.
 */
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
        this->_text_x->setPlainText(
            QString::asprintf("%0.2f", pos.x() / 100.0));

        this->_text_y->show();
        this->_text_y->setPlainText(
            QString::asprintf("%0.2f", pos.y() / 100.0));

        this->_text_y->setPos(100.00, pos.y() + 13.00);

        this->_line_y->setLine(QLineF(0.00, pos.y(), 100.00, pos.y()));
    }
    // Hide text values if outside of preview range
    else
    {
        this->_text_x->hide();
        this->_text_y->hide();
    }

    // If we are clicking (i.e. assumed editing control points, hide preview
    // values)
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
        // between a specific bezier curve, if two beziers, we are claming an
        // end point and its matching control points)
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

/**
 * mousePressEvent
 * 
 * When the mouse is pressed it the user selects a control point its reference
 * is saved as a pointer (along with any other needed objects) for manipulating
 * the curve.
 * 
 * @param QMouseEvent* event : The mouse press event.
 * 
 * @signals curveChanged
 */
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
                // If pressing shift, we keep looping to find a control, not end
                // point, thus need to confirm we found a point, rather than
                // break, breaking loops
                bool found = false;
                // Find the containing beziers
                for (int j = 0; j < (int)this->_beziers.size(); j++)
                {
                    Bezier *b = &this->_beziers[j];

                    // If we are control clicking and its a control point,
                    // collapse the control point to its coresponding end point
                    if (event->modifiers() == Qt::ControlModifier
                        && (b->control_0 == e || b->control_1 == e))
                    {
                        if (b->control_0 == e)
                            e->setPos(b->end_0->pos());

                        if (b->control_1 == e)
                            e->setPos(b->end_1->pos());

                        b->draw();
                        emit this->curveChanged();
                        return;
                    }

                    // If its an end point, and we are not shift clicking
                    // (ignore endpoints) Setup controls to be affected
                    if (b->end_0 == e
                        && !event->modifiers().testFlag(Qt::ShiftModifier))
                    {
                        this->_control = e;
                        this->_extra_1 = b->control_0;
                        this->_bezier_1 = b;
                        found = true;
                        break;
                    }
                    else if (b->end_1 == e
                             && !event->modifiers().testFlag(Qt::ShiftModifier))
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

/**
 * mouseReleaseEvent
 * 
 * When the mouse button is released the pointers to the control elements being
 * manipulated are set to null, and the curve changed event is emitted to allow
 * for calculations with the curve to begin again.
 * 
 * @param QMouseEvent* event : The mouse release event. (unused)
 * 
 * @signals curveChanged
 */
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

/**
 * enterEvent
 * 
 * When the mouse enters the widget the editing controls are shown.
 * 
 * @param QEvent* event : Unused event.
 */
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

/**
 * leaveEvent
 * 
 * When the mouse leaves the widget the control elements are hidden.
 * 
 * @param QEvent* event : Unused event.
 */
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

/**
 * keyPressEvent
 * 
 * When the user pressed the X key while hovering over a control point that
 * control point is placed at the exact same spot as its corresponding end
 * point.
 * 
 * @param QKeyEvent* event : The key press event.
 */
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
            QPointF end =
                this->_beziers[(int)this->_beziers.size() - 1].end_1->pos();

            this->_line_start->setLine(QLineF(0.00,
                                              start.y(),
                                              start.x(),
                                              start.y()));

            this->_line_end->setLine(QLineF(end.x(), end.y(), 100.00, end.y()));
        }
    }
}

/**
 * mouseDoubleClickEvent
 * 
 * When the user double clicks on the widget the bezier curve within the x pos
 * of the click is split calling its split function.
 * 
 * @param QMouseEvent* event : The mouse double click event.
 * 
 * @signals curveChanged
 */
void BezierEditWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPointF pos = this->mapToScene(event->pos());
    // Find bezier in the x range
    for (int i = 0; i < (int)this->_beziers.size(); i++)
    {
        // Split the bezier into two
        if (this->_beziers[i].end_0->pos().x() <= pos.x()
            && pos.x() < this->_beziers[i].end_1->pos().x())
        {
            Bezier *beziers = this->_beziers[i].split();
            this->_beziers[i] = beziers[0];
            this->_beziers.insert(this->_beziers.begin() + i + 1, beziers[1]);
            emit this->curveChanged();
            return;
        }
    }
}