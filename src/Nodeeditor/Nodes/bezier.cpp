#include "bezier.h"

ConverterBezierCurveNode::ConverterBezierCurveNode()
{
    this->_widget = new BezierCurveEditWidget();
}
ConverterBezierCurveNode::~ConverterBezierCurveNode() {}

void ConverterBezierCurveNode::created()
{
    Q_CHECK_PTR(this->_widget);
    QObject::connect(this->_widget, &BezierCurveEditWidget::curveChanged, [this]() {
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
    return QString("Bézier");
}

// Title shown in the selection list
QString ConverterBezierCurveNode::name() const
{
    return QString("Bézier transform");
}

// The embedded control widget
QWidget *ConverterBezierCurveNode::embeddedWidget()
{
    Q_CHECK_PTR(this->_widget);
    return this->_widget;
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
    return std::make_shared<IntensityMapData>(this->_output);
}

// Save and load the node
QJsonObject ConverterBezierCurveNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    return data;
}
void ConverterBezierCurveNode::restore(QJsonObject const &data)
{
    Q_UNUSED(data);
}

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

void ConverterBezierCurveNode::_generate()
{
    Q_CHECK_PTR(this->_input);
    Q_CHECK_PTR(this->_widget);
    IntensityMap map = this->_input->intensityMap();

    this->_output = IntensityMap(map.width, map.height);

    for (int y = 0; y < map.height; y++)
        for (int x = 0; x < map.width; x++)
            this->_output.append(this->_widget->f(map.at(x, y)));

    emit this->dataUpdated(0);
}