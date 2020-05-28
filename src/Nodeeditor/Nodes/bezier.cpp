#include "bezier.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

// Converts intensity map bay taking in a height value (x) and applying a curve
// function f(x) to get a new height value y = f(x).
// Bezier curve control points are limited to be within in the end points x values
// thus ensuring they do not violate the vertical line test, the y = f(x) is solved
// using line intersection against the line segments of the bezier curve
// the widget has 2048 samples within in the range of 0 to 1 when drawing the lines
// (each line is from between these samples), this helps to keep high accuracy when
// doing the vertical line intersection. No real way to get a perfect f(x) function.
ConverterBezierCurveNode::ConverterBezierCurveNode()
{
    this->_widget = new BezierEditWidget();
    this->_shared_widget = new BezierEditWidget();
}
ConverterBezierCurveNode::~ConverterBezierCurveNode() {}

// Node is created, attach listeners
void ConverterBezierCurveNode::created()
{
    Q_CHECK_PTR(this->_widget);
    Q_CHECK_PTR(this->_shared_widget);
    QObject::connect(this->_widget, &BezierEditWidget::curveChanged, [this]() {
        Q_CHECK_PTR(this->_widget);
        Q_CHECK_PTR(this->_shared_widget);
        this->_shared_widget->restore(this->_widget->save());
        if (this->_set)
        {
            this->_generate();
            return;
        }
    });
    QObject::connect(this->_shared_widget, &BezierEditWidget::curveChanged, [this]() {
        Q_CHECK_PTR(this->_widget);
        Q_CHECK_PTR(this->_shared_widget);
        this->_widget->restore(this->_shared_widget->save());
        if (this->_set)
        {
            this->_generate();
            return;
        }
    });
}

// Title shown at the top of the node
QString ConverterBezierCurveNode::caption() const
{
    return QString("Slope and Curve");
}

// Title shown in the selection list
QString ConverterBezierCurveNode::name() const
{
    return QString("Slope and Curve transform");
}

// The embedded control widget
QWidget *ConverterBezierCurveNode::embeddedWidget()
{
    Q_CHECK_PTR(this->_widget);
    return this->_widget;
}
// The shared control widget
QWidget *ConverterBezierCurveNode::sharedWidget()
{
    Q_CHECK_PTR(this->_shared_widget);
    return this->_shared_widget;
}

// Get the number of ports (1 output, 4 input)
unsigned int ConverterBezierCurveNode::nPorts(QtNodes::PortType port_type) const
{
    Q_UNUSED(port_type);
    return 1;
}

// Get the port datatype
QtNodes::NodeDataType ConverterBezierCurveNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    return IntensityMapData().type();
}

// Get the output data (the VectorMapData)
std::shared_ptr<QtNodes::NodeData> ConverterBezierCurveNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return std::make_shared<IntensityMapData>(this->_output);
}

// Save and load the node
QJsonObject ConverterBezierCurveNode::save() const
{
    Q_CHECK_PTR(this->_widget);

    QJsonObject data;
    data["name"] = this->name();
    std::vector<QPointF> save_data = this->_widget->save();
    QJsonArray values;

    for (int i = 0; i < (int)save_data.size(); i++)
    {
        QJsonObject point;
        point["x"] = save_data[i].x();
        point["y"] = save_data[i].y();
        values << point;
    }

    data["values"] = values;
    return data;
}
void ConverterBezierCurveNode::restore(QJsonObject const &data)
{
    Q_CHECK_PTR(this->_widget);
    Q_CHECK_PTR(this->_shared_widget);

    std::vector<QPointF> save_data;
    QJsonArray values = data["values"].toArray();

    for (int i = 0; i < values.count(); i++)
    {
        QJsonObject point = values.at(i).toObject();
        double x = point["x"].toDouble(0.00);
        double y = point["y"].toDouble(0.00);
        save_data.push_back(QPointF(x, y));
    }

    this->_widget->restore(save_data);
    this->_shared_widget->restore(save_data);
}

// When the input is deleted, use default output
void ConverterBezierCurveNode::inputConnectionDeleted(QtNodes::Connection const &connection)
{
    Q_UNUSED(connection);
    this->_output = IntensityMap(1, 1, 1.00);
    this->_set = true;
    emit this->dataUpdated(0);
}

// Set the input node
void ConverterBezierCurveNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
{
    Q_UNUSED(port);

    if (node_data)
    {
        this->_input = std::dynamic_pointer_cast<IntensityMapData>(node_data);
        this->_set = true;
        this->_generate();
    }
}

// Generate output
void ConverterBezierCurveNode::_generate()
{
    Q_CHECK_PTR(this->_input);
    Q_CHECK_PTR(this->_widget);
    IntensityMap map = this->_input->intensityMap();

    if (map.usingFill())
    {
        double v = this->_widget->valueAt(map.at(0, 0));
        this->_output = IntensityMap(map.width, map.height, v);
    }
    else
    {
        this->_output = IntensityMap(map.width, map.height);

        for (int y = 0; y < map.height; y++)
            for (int x = 0; x < map.width; x++)
                this->_output.append(this->_widget->valueAt(map.at(x, y)));
    }

    emit this->dataUpdated(0);
}