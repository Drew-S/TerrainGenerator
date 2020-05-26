#include <QGraphicsView>
#include <QResizeEvent>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QEvent>
#include <QPoint>
#include <QColor>

#include <vector>

typedef struct
{
    double x;
    double y;
    double t;
} XLoc;

class BezierCurveEditWidget : public QGraphicsView
{
    Q_OBJECT
public:
    BezierCurveEditWidget(float p = 1.0f,
                          float s = 0.67f,
                          float g = 0.33f,
                          float r = 10.0f,
                          double size = 100.00,
                          float text = 0.5f,
                          double gap = 50.00,
                          int samples = 1024,
                          QWidget *parent = nullptr);
    ~BezierCurveEditWidget();

    double f(double x);

signals:
    void curveChanged();

public slots:
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

    double _size_scale = 100.00;
    double _gap = 50.00;
    int _samples = 4096;

    float _text_scale = 0.5f;

    std::map<int, XLoc> _table;

    std::vector<QGraphicsLineItem *> _curve;

    QPoint _prev;

    QGraphicsEllipseItem *_c_1;
    QGraphicsEllipseItem *_c_2;

    QGraphicsLineItem *_line_0;
    QGraphicsLineItem *_line_1;

    QGraphicsLineItem *_line_x;
    QGraphicsLineItem *_line_y;

    QGraphicsTextItem *_text_x;
    QGraphicsTextItem *_text_y;

    bool _move_c_1 = false;
    bool _move_c_2 = false;

    QPointF _zero;
    QPointF _one;
};