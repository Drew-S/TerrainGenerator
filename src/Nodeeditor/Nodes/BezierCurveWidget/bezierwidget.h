#include <QGraphicsView>
#include <QResizeEvent>
#include <QGraphicsEllipseItem>
#include <QEvent>
#include <QPoint>
#include <QColor>

#include <vector>

class BezierCurveEditWidget : public QGraphicsView
{
    Q_OBJECT
public:
    BezierCurveEditWidget(QWidget *parent = nullptr);
    ~BezierCurveEditWidget();

    double f(double x);

signals:
    void curveChanged();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    double _bezier(double t, double a, double b, double c, double d);
    void _draw();
    void _generate();

    float _primary_line_width = 1.0f;
    float _secondary_line_width = 0.67f;
    float _grid_line_width = 0.33f;
    float _radius = 10.0f;

    std::map<int, double> _table;

    std::vector<QGraphicsLineItem *> _curve;
    QPoint _prev;

    double _size_scale = 100.00;
    double _gap = 50.00;
    int _accuracy = 1024;

    float _text_scale = 0.5f;

    QGraphicsEllipseItem *_c_0;
    QGraphicsEllipseItem *_c_1;
    QGraphicsEllipseItem *_c_2;
    QGraphicsEllipseItem *_c_3;

    QGraphicsLineItem *_line_0;
    QGraphicsLineItem *_line_1;

    bool _move_c_1 = false;
    bool _move_c_2 = false;
};