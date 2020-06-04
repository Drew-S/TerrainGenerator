#include "invertintensity.h"

#include <QDebug>

/**
 * ConverterInvertIntensityNode
 * 
 * Create the node.
 */
ConverterInvertIntensityNode::ConverterInvertIntensityNode()
{
    qDebug("Created intensity inverting node");
}

/**
 * caption
 * 
 * Return a string that is displayed on the node and in the properties.
 * 
 * @returns QString : The caption.
 */
QString ConverterInvertIntensityNode::caption() const
{
    return QString("Invert");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString ConverterInvertIntensityNode::name() const
{
    return QString("Invert");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram.
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *ConverterInvertIntensityNode::embeddedWidget()
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
unsigned int
ConverterInvertIntensityNode::nPorts(QtNodes::PortType port_type) const
{
    Q_UNUSED(port_type);
    return 1;
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
ConverterInvertIntensityNode::dataType(QtNodes::PortType port_type,
                                       QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    return IntensityMapData().type();
}

/**
 * save
 * 
 * Saves the state of the node into a QJsonObject for the system to save to
 * file.
 * 
 * @returns QJsonObject : The saved state of the node.
 */
QJsonObject ConverterInvertIntensityNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    return data;
}

/**
 * restore
 * 
 * Restores the state of the node from a provided json object. (unused data)
 * 
 * @param QJsonObject const& data : The data to restore from.
 */
void ConverterInvertIntensityNode::restore(QJsonObject const &data)
{
    Q_UNUSED(data);
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
ConverterInvertIntensityNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return std::make_shared<IntensityMapData>(this->_output);
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
void ConverterInvertIntensityNode::setInData(
    std::shared_ptr<QtNodes::NodeData> node_data,
    QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    if (node_data)
    {
        this->_input = std::dynamic_pointer_cast<IntensityMapData>(node_data);
        this->_set = true;
        this->_generate();
    }
}

/**
 * inputConnectionDeleted @slot
 * 
 * Called when an input connection is deleted, this usually resets some data and
 * regenerates the output data.
 * 
 * @param QtNodes::Connection const& connection : The connection being deleted.
 * 
 * @signals dataUpdated
 */
void ConverterInvertIntensityNode::inputConnectionDeleted(
    QtNodes::Connection const &connection)
{
    Q_UNUSED(connection);
    this->_set = false;
    this->_output = IntensityMap(1, 1, 1.00);
    emit this->dataUpdated(0);
}

/**
 * _generate
 * 
 * Generates the output data from the supplied and available data.
 * 
 * @signals dataUpdated
 */
void ConverterInvertIntensityNode::_generate()
{
    qDebug("Inverting intensity map");
    Q_CHECK_PTR(this->_input);
    IntensityMap map = this->_input->intensityMap();
    this->_output = IntensityMap(map.width, map.height);
    for (int y = 0; y < map.height; y++)
        for (int x = 0; x < map.width; x++)
            this->_output.append(1.00 - map.at(x, y));

    emit this->dataUpdated(0);
}