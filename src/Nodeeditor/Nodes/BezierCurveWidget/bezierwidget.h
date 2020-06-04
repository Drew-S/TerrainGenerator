#pragma once

#include <vector>

#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPointF>
#include <QResizeEvent>

/**
 * Bezier
 * 
 * The container that houses a single segment of the overall bezier curve.
 * This manages the two end points, the two control points, the two lines
 * betwen them, and the curve itself.
 */
class Bezier
{
public:
    // Create the default bezier object
    Bezier();

    // Create with these values for the different colours
    Bezier(QPen l_pen,                // control to end point line
           QPen c_pen,                // control outline
           QBrush c_brush,            // control fill
           QPen e_pen,                // end point outline
           QBrush e_brush,            // end point fill
           QPen cu_pen,               // curve line
           float r,                   // end/control point radius
           float c_thick,             // control/endpoint outline thickness
           float l_width,             // control to end point line thickness
           float c_width,             // curve thickness
           QPointF c_0,               // control point 0 (matches with end point
                                      // 0)
           QPointF c_1,               // control point 1 (matches with end point
                                      // 1)
           QGraphicsEllipseItem *e_0, // end point 0
           QGraphicsEllipseItem *e_1, // end point 1
           QGraphicsScene *s);

    // Get the (x,y) intersection point for a line, used for getting f(x)
    bool intersect(QLineF line, QPointF *pos);

    // Draws the curve, updates control lto end point lines
    void draw();

    // Hide the control elements
    void hide();

    // Show the control elements
    void show();

    // Clears the curve line (for redrawing updated curve)
    void clear();

    // Removes the control values from the widget, for using restored data
    void remove(bool keep_ellipses = false);

    // Splits a curve at t=0.5 into two smaller bezier curves
    Bezier *split();

    // Drawing details
    QPen line_pen;
    QPen control_pen;
    QBrush control_brush;
    QPen end_pen;
    QBrush end_brush;
    QPen curve_pen;

    float radius;
    float control_thick;
    float line_width;
    float curve_width;

    // Graphic elements
    std::vector<QGraphicsLineItem *> curve;
    QGraphicsScene *scene = nullptr;

    QGraphicsEllipseItem *end_0 = nullptr;
    QGraphicsEllipseItem *end_1 = nullptr;
    QGraphicsEllipseItem *control_0 = nullptr;
    QGraphicsEllipseItem *control_1 = nullptr;

    QGraphicsLineItem *line_0 = nullptr;
    QGraphicsLineItem *line_1 = nullptr;
};

/**
 * BezierFactory
 * 
 * Factory class for creating Bezier objects that ensures the entirety of the
 * overall bezier curve is drawn with the same stylings.
 */
class BezierFactory
{
public:
    // Create a default factory
    BezierFactory(QGraphicsScene *s);

    // Create a customized factory
    BezierFactory(QPen l_pen,
                  QPen c_pen,
                  QBrush c_brush,
                  QPen e_pen,
                  QBrush e_brush,
                  QPen cu_pen,
                  float r = 3.0f,
                  float c_thick = 0.25f,
                  float l_width = 0.5f,
                  float c_width = 1.0f,
                  QGraphicsScene *s = nullptr);

    // Creates a new bezier with same pen and brushes
    Bezier newBezier(QPointF c_0,
                     QPointF c_1,
                     QGraphicsEllipseItem *e_0,
                     QGraphicsEllipseItem *e_1);

    // Create an ellipse (since end points are shared between beziers)
    QGraphicsEllipseItem *newEllipse(QPointF pos);

    // Graphics reference
    QGraphicsScene *scene = nullptr;

    // Drawing details
    QPen line_pen;
    QPen control_pen;
    QBrush control_brush;
    QPen end_pen;
    QBrush end_brush;
    QPen curve_pen;

    float radius = 8.0f;
    float control_thick = 0.25f;
    float line_width = 0.5f;
    float curve_width = 1.0f;
};

/**
 * BezierEditWidget
 * 
 * A custom widget that lets users draw curves and slopes using bezier curves
 * that are constrained to ensure the vertical line test does not fail thus
 * creating a widget that can apply the function y = f(x).
 */
class BezierEditWidget : public QGraphicsView
{
    Q_OBJECT
public:
    // Create the widget, divs are the number of grid squares for preview
    BezierEditWidget(int divs = 4, QWidget *parent = nullptr);

    // Get the intersection value for a line
    bool intersect(QLineF line, QPointF *pos);

    // Get an intersection value for a vertical line at x = value
    double valueAt(double value);

    // Save the curves control points
    std::vector<QPointF> save();

    // Restores the curves controls points
    void restore(std::vector<QPointF> values);

signals:
    // When the curve is updated (after letting go of control point)
    void curveChanged();

protected:
    // Resize to fit scene in widget
    void resizeEvent(QResizeEvent *event);

    // For preview lines and moving points
    void mouseMoveEvent(QMouseEvent *event);

    // For selecting points
    void mousePressEvent(QMouseEvent *event);

    // For finishing updating
    void mouseReleaseEvent(QMouseEvent *event);

    // Only show preview lines when hovering over widget
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    // On x key delete extra beziers (delete key does not work, so use x
    // instead, similar to Blender)
    void keyPressEvent(QKeyEvent *event);

    // Double click on a curve to split it
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    // Clears the curves for redrawing (restore)
    void _clear();

    // Factory for creating curves
    BezierFactory *_factory;
    // The curves
    std::vector<Bezier> _beziers;

    // Previous point for relative mouse movement
    QPointF _prev;

    // Preview lines
    QGraphicsLineItem *_line_x = nullptr;
    QGraphicsLineItem *_line_y = nullptr;

    // Preview values
    QGraphicsTextItem *_text_x = nullptr;
    QGraphicsTextItem *_text_y = nullptr;

    // The active control element
    QGraphicsEllipseItem *_control = nullptr;

    // The extra control elements (moving an end point moves its partner control
    // points)
    QGraphicsEllipseItem *_extra_0 = nullptr;
    QGraphicsEllipseItem *_extra_1 = nullptr;

    // The bezier curves be affected by the control point (for redrawing and 
    // checks)
    Bezier *_bezier_0 = nullptr;
    Bezier *_bezier_1 = nullptr;

    // The overflow lines (these continue from the first and last ends of the
    // curve to 0 and 1)
    QGraphicsLineItem *_line_start = nullptr;
    QGraphicsLineItem *_line_end = nullptr;
};

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
double bezier(double t, double a, double b, double c, double d);

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
QPointF bezier(double t, QPointF a, QPointF b, QPointF c, QPointF d);