#include "normalize.h"

#include <math.h>

#include <QDebug>

/**
 * ConverterNormalizeNode
 * 
 * Creates the node. No UI with this one.
 */
ConverterNormalizeNode::ConverterNormalizeNode()
{
    qDebug("Created normalize node");
}

/**
 * caption
 * 
 * Return a string that is displayed on the node and in the properties.
 * 
 * @returns QString : The caption.
 */
QString ConverterNormalizeNode::caption() const
{
    return QString("Normalize");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString ConverterNormalizeNode::name() const
{
    return QString("Normalize");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram. (returns nullptr, no widget to manipulate)
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *ConverterNormalizeNode::embeddedWidget()
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
unsigned int ConverterNormalizeNode::nPorts(QtNodes::PortType port_type) const
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
ConverterNormalizeNode::dataType(QtNodes::PortType port_type,
                                 QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    return VectorMapData().type();
}

/**
 * save
 * 
 * Saves the state of the node into a QJsonObject for the system to save to
 * file.
 * 
 * @returns QJsonObject : The saved state of the node.
 */
QJsonObject ConverterNormalizeNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    return data;
}

/**
 * restore
 * 
 * Restores the state of the node from a provided json object. (Nothing to
 * restore, node only works with port data)
 * 
 * @param QJsonObject const& data : The data to restore from.
 */
void ConverterNormalizeNode::restore(QJsonObject const &data)
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
ConverterNormalizeNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return std::make_shared<VectorMapData>(this->_output);
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
void ConverterNormalizeNode::setInData(
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
 * inputConnectionDeleted @slot
 * 
 * Called when an input connection is deleted, this usually resets some data and
 * regenerates the output data.
 * 
 * @param QtNodes::Connection const& connection : The connection being deleted.
 * 
 * @signals dataUpdated
 */
void ConverterNormalizeNode::inputConnectionDeleted(
    QtNodes::Connection const &connection)
{
    Q_UNUSED(connection);
    this->_set = false;
    this->_output = VectorMap(1.00, 1.00, glm::dvec4(0.00, 0.00, 0.00, 1.00));
    emit this->dataUpdated(0);
}

/**
 * normalize
 * 
 * Normalizes the input a vector.
 * 
 * @param glm::dvec4 a : The vector to be normalized.
 * @param glm::dvec4 b : (unused) Extra vector to match requirements for vector
 *                       map transform function.
 * 
 * @returns glm::dev4 : The normalized vector.
 */
glm::dvec4 ConverterNormalizeNode::normalize(glm::dvec4 a, glm::dvec4 b)
{
    Q_UNUSED(b);
    double sum = 1.00 / sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z) + (a.w * a.w));
    return a * sum;
}

/**
 * _generate
 * 
 * Generates the output data from the supplied and available data.
 * 
 * @signals dataUpdated
 */
void ConverterNormalizeNode::_generate()
{
    qDebug("Normalizing vector map");
    Q_CHECK_PTR(this->_input);
    VectorMap map = this->_input->vectorMap();
    this->_output = map.transform(&ConverterNormalizeNode::normalize, glm::dvec4());
    emit this->dataUpdated(0);
}