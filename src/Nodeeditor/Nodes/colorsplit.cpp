#include "colorsplit.h"

#include "../Datatypes/vectormap.h"

#include <glm/vec4.hpp>

ConverterColorSplit::ConverterColorSplit() {}
ConverterColorSplit::~ConverterColorSplit() {}

// Caption name shown in the node
QString ConverterColorSplit::caption() const
{
    return QString("Color Splitter");
}

// The name of the node
QString ConverterColorSplit::name() const
{
    return QString("ColorSplitNode");
}
void ConverterColorSplit::name(QString name)
{
    (void)name;
}

// The embedded widget (none)
QWidget *ConverterColorSplit::embeddedWidget()
{
    return nullptr;
}

// The model name of the node
QString ConverterColorSplit::modelName()
{
    return QString("Color Split Node");
}

// Get the number of ports (1 in, 4 out)
unsigned int ConverterColorSplit::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 1 : 4;
}

// Get the port data type, VectorMap in, IntensityMap outputs, outputs are labelled independently
QtNodes::NodeDataType ConverterColorSplit::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    // (void)port_index;
    // return port_type == QtNodes::PortType::In ? VectorMapData().type() : IntensityMapData().type();
    if (port_type == QtNodes::PortType::In)
        return VectorMapData().type();

    QtNodes::NodeDataType type = IntensityMapData().type();

    switch ((int)port_index)
    {
    case 0:
        return {type.id, "red"};
        break;
    case 1:
        return {type.id, "green"};
        break;
    case 2:
        return {type.id, "blue"};
        break;
    case 3:
        return {type.id, "alpha"};
        break;
    }
}

// When the input is set mark as set and split the channels out
void ConverterColorSplit::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
{
    (void)port;
    if (node_data)
    {
        this->_input = std::dynamic_pointer_cast<VectorMapData>(node_data);
        this->_set = true;
        this->_generate();
    }
}

// Get a channel for one of the output channels
std::shared_ptr<QtNodes::NodeData> ConverterColorSplit::outData(QtNodes::PortIndex port)
{
    switch ((int)port)
    {
    case 0:
        return std::make_shared<IntensityMapData>(this->_red);
        break;
    case 1:
        return std::make_shared<IntensityMapData>(this->_green);
        break;
    case 2:
        return std::make_shared<IntensityMapData>(this->_blue);
        break;
    case 3:
        return std::make_shared<IntensityMapData>(this->_alpha);
        break;
    }
}

// Save the model for file
QJsonObject ConverterColorSplit::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    return data;
}

// Load the model (no internal data to load)
void ConverterColorSplit::restore(QJsonObject const &data)
{
    (void)data;
}

// When the connection is deleted reset the model to use default outputs
void ConverterColorSplit::inputConnectionDeleted(QtNodes::Connection const &connection)
{
    this->_set = false;
    this->_red = IntensityMap(1, 1, 1.00);
    this->_green = IntensityMap(1, 1, 1.00);
    this->_blue = IntensityMap(1, 1, 1.00);
    this->_alpha = IntensityMap(1, 1, 1.00);
    this->_generate();
}

// Splits the channels outputs
void ConverterColorSplit::_generate()
{
    VectorMap map = this->_input->vectorMap();

    this->_red = IntensityMap(map.width, map.height);
    this->_green = IntensityMap(map.width, map.height);
    this->_blue = IntensityMap(map.width, map.height);
    this->_alpha = IntensityMap(map.width, map.height);

    for (int y = 0; y < map.height; y++)
    {
        for (int x = 0; x < map.width; x++)
        {
            this->_red.append(map.at(x, y).r);
            this->_green.append(map.at(x, y).g);
            this->_blue.append(map.at(x, y).b);
            this->_alpha.append(map.at(x, y).a);
        }
    }

    emit this->dataUpdated(0);
    emit this->dataUpdated(1);
    emit this->dataUpdated(2);
    emit this->dataUpdated(3);
}
