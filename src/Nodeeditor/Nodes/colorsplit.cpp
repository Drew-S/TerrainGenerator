#include "colorsplit.h"

#include <QDebug>

#include "../Datatypes/vectormap.h"

#include <glm/vec4.hpp>

/**
 * ConverterColorSplitNode
 * 
 * Creates a new node, this does not have any widgets.
 */
ConverterColorSplitNode::ConverterColorSplitNode()
{
    qDebug("Creating colour split node");
}

/**
 * caption
 * 
 * Return a string that is displayed on the node and in the properties.
 * 
 * @returns QString : The caption.
 */
QString ConverterColorSplitNode::caption() const
{
    return QString("Split Colour Channels");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString ConverterColorSplitNode::name() const
{
    return QString("Colour Split");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram. (none)
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *ConverterColorSplitNode::embeddedWidget()
{
    return nullptr;
}

/**
 * nPorts
 * 
 * Returns the number of ports the node has per type of port.
 * 
 * @param QtNodes::PortType port_type : The type of port to get the number of
 *                                      ports. QtNodes::PortType::In (input),
 *                                      QtNodes::PortType::Out (output)
 * 
 * @returns unsigned int : The number of ports.
 */
unsigned int ConverterColorSplitNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 1 : 4;
}

/**
 * dataType
 * 
 * Returns the data type for each of the ports.
 * 
 * @param QtNodes::PortType port_type : The type of port (in or out).
 * @param QtNodes::PortIndex port_index : The port index on each side.
 * 
 * @returns QtNodes::NodeDataType : The type of data the port provides/accepts.
 */
QtNodes::NodeDataType
ConverterColorSplitNode::dataType(QtNodes::PortType port_type,
                                  QtNodes::PortIndex port_index) const
{
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
    default:
        Q_UNREACHABLE();
        break;
    }
    Q_UNREACHABLE();
}

/**
 * setInData
 * 
 * Sets the input data on a port.
 * 
 * @param std::shared_ptr<QtNodes::NodeData> node_data : The shared pointer data
 *                                                       being inputted.
 * @param QtNodes::PortIndex port : The port the data is being set on.
 */
void ConverterColorSplitNode::setInData(
    std::shared_ptr<QtNodes::NodeData> node_data,
    QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    if (node_data)
    {
        this->_input = std::dynamic_pointer_cast<VectorMapData>(node_data);
        this->_set = true;
        this->_generate();
    }
}

/**
 * outData
 * 
 * Returns a shared pointer for transport along a connection to another node.
 * 
 * @param QtNodes::PortIndex port : The port to get data from.
 * 
 * @returns std::shared_ptr<QtNodes::NodeData> : The shared output data.
 */
std::shared_ptr<QtNodes::NodeData>
ConverterColorSplitNode::outData(QtNodes::PortIndex port)
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
    default:
        Q_UNREACHABLE();
        break;
    }
}

/**
 * save
 * 
 * Saves the state of the node into a QJsonObject for the system to save to
 * file.
 * 
 * @returns QJsonObject : The saved state of the node.
 */
QJsonObject ConverterColorSplitNode::save() const
{
    qDebug("Saving color split node");
    QJsonObject data;
    data["name"] = this->name();
    return data;
}

/**
 * restore
 * 
 * Restores the state of the node from a provided json object.
 * 
 * @param QJsonObject const& data : The data to restore from.
 */
void ConverterColorSplitNode::restore(QJsonObject const &data)
{
    Q_UNUSED(data);
    qDebug("Restoring color split node");
}

/**
 * inputConnectionDeleted @slot
 * 
 * Called when an input connection is deleted, this usually resets some data and
 * regenerates the output data.
 * 
 * @param QtNodes::Connection const& connection : The connection being deleted.
 */
void ConverterColorSplitNode::inputConnectionDeleted(
    QtNodes::Connection const &connection)
{
    Q_UNUSED(connection);
    this->_set = false;
    this->_red = IntensityMap(1, 1, 1.00);
    this->_green = IntensityMap(1, 1, 1.00);
    this->_blue = IntensityMap(1, 1, 1.00);
    this->_alpha = IntensityMap(1, 1, 1.00);
    this->_generate();
}

/**
 * _generate
 * 
 * Generates the output data from the supplied and available data.
 * 
 * @signals dataUpdated
 */
void ConverterColorSplitNode::_generate()
{
    qDebug("Splitting color channels to output");
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
