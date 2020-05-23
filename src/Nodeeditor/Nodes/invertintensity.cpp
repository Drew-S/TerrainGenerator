#include "invertintensity.h"

ConverterInvertIntensityNode::ConverterInvertIntensityNode() {}

ConverterInvertIntensityNode::~ConverterInvertIntensityNode() {}

// The name of the node in the widget
QString ConverterInvertIntensityNode::caption() const
{
    return QString("Invert");
}

// The name of the node in the selection list
QString ConverterInvertIntensityNode::name() const
{
    return QString("Invert");
}

// No control widget, simple inverse function
QWidget *ConverterInvertIntensityNode::embeddedWidget()
{
    return nullptr;
}

// 1 in and out port
unsigned int ConverterInvertIntensityNode::nPorts(QtNodes::PortType port_type) const
{
    Q_UNUSED(port_type);
    return 1;
}

// The port types are intensity maps
QtNodes::NodeDataType ConverterInvertIntensityNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    return IntensityMapData().type();
}

// Save the node
QJsonObject ConverterInvertIntensityNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    return data;
}

// Restore the node
void ConverterInvertIntensityNode::restore(QJsonObject const &data)
{
    Q_UNUSED(data);
}

// Return the generated output
std::shared_ptr<QtNodes::NodeData> ConverterInvertIntensityNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return std::make_shared<IntensityMapData>(this->_output);
}

// Set the input intensity map
void ConverterInvertIntensityNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    if (node_data)
    {
        this->_input = std::dynamic_pointer_cast<IntensityMapData>(node_data);
        this->_set = true;
        this->_generate();
    }
}

// On delete set to use the default output
void ConverterInvertIntensityNode::inputConnectionDeleted(QtNodes::Connection const &connection)
{
    Q_UNUSED(connection);
    this->_set = false;
    this->_output = IntensityMap(1, 1, 1.00);
    emit this->dataUpdated(0);
}

// Inverts an intensity map v' = (1 - v) since the application should be working with values
// between 0-1
void ConverterInvertIntensityNode::_generate()
{
    Q_CHECK_PTR(this->_input);
    IntensityMap map = this->_input->intensityMap();
    IntensityMap out(map.width, map.height);
    for (int y = 0; y < map.height; y++)
        for (int x = 0; x < map.width; x++)
            out.append(1.00 - map.at(x, y));

    emit this->dataUpdated(0);
}