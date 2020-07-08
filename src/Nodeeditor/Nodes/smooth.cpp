#include "smooth.h"

#include <QDebug>

#include "Globals/settings.h"

/**
 * ConverterSmoothNode
 * 
 * Creates the node and creates the UI.
 */
ConverterSmoothNode::ConverterSmoothNode()
{
    qDebug("Creating Smooth Node");
}

/**
 * caption
 * 
 * Return a string that is displayed on the node and in the properties.
 * 
 * @returns QString : The caption.
 */
QString ConverterSmoothNode::caption() const
{
    return QString("Smooth");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString ConverterSmoothNode::name() const
{
    return QString("Smooth");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram. (No Widget)
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *ConverterSmoothNode::embeddedWidget()
{
    return nullptr;
}

/**
 * sharedWidget
 * 
 * Returns a pointer to the widget that gets displayed in the properties panel.
 * 
 * @returns QWidget* : The shared widget.
 */
QWidget *ConverterSmoothNode::sharedWidget()
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
unsigned int ConverterSmoothNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 1 : 1;
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
ConverterSmoothNode::dataType(QtNodes::PortType port_type,
                            QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    return IntensityMapData().type();
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
void ConverterSmoothNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data,
                                  QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    if (node_data && (this->_input = std::dynamic_pointer_cast<IntensityMapData>(node_data)))
    {
        this->_set = true;
        this->_generate();
    }
}

/**
 * _generate
 * 
 * Generates the output data from the supplied and available data.
 * 
 * @signals dataUpdated
 */
void ConverterSmoothNode::_generate()
{
    Q_CHECK_PTR(this->_input);
    IntensityMap map = this->_input->intensityMap();
    this->_output = IntensityMap(map.width, map.height);

    double cor = this->v;
    double cen = cor + 3.00;
    double mid = cen * 5.00;

    double K[3][3] = {
        {cor, cen, cor},
        {cen, mid, cen},
        {cor, cen, cor}};

    double line = cen + (2.00 * cor);

    for (int y = 0; y < map.height; y++)
    {
        for (int x = 0; x < map.width; x++)
        {
            double M[3][3] = {
                {map.at(x - 1, y - 1), map.at(x, y - 1), map.at(x + 1, y - 1)},
                {map.at(x - 1, y), map.at(x, y), map.at(x + 1, y)},
                {map.at(x - 1, y + 1), map.at(x, y + 1), map.at(x + 1, y + 1)}};

            double div = mid + (4.00 * cen) + (4.00 * cor);

            if (x <= 0 || x >= map.width - 1)
                div -= line;

            if (y <= 0 || y >= map.height - 1)
                div -= line;

            if ((y <= 0 || y >= map.height - 1)
                && (x <= 0 || x >= map.width - 1))
                div += cor;

            double v = (M[0][0] * K[0][0])
                     + (M[1][0] * K[1][0])
                     + (M[2][0] * K[2][0])
                     + (M[0][1] * K[0][1])
                     + (M[1][1] * K[1][1])
                     + (M[2][1] * K[2][1])
                     + (M[0][2] * K[0][2])
                     + (M[1][2] * K[1][2])
                     + (M[2][2] * K[2][2]);

            this->_output.append(v / div);
        }
    }
    emit this->dataUpdated(0);
}

/**
 * inputConnectionDeleted @slot
 * 
 * Called when an input connection is deleted, this usually resets some data and
 * regenerates the output data.
 * 
 * @param QtNodes::Connection const& connection : The connection being deleted.
 */
void ConverterSmoothNode::inputConnectionDeleted(
    QtNodes::Connection const &connection)
{
    Q_UNUSED(connection);
    this->_set = false;

    emit this->dataUpdated(0);
}

/**
 * save
 * 
 * Saves the state of the node into a QJsonObject for the system to save to
 * file.
 * 
 * @returns QJsonObject : The saved state of the node.
 */
QJsonObject ConverterSmoothNode::save() const
{
    qDebug("Saving math node");
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
void ConverterSmoothNode::restore(QJsonObject const &data)
{
    Q_UNUSED(data);
    qDebug("Restoring math node");
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
ConverterSmoothNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return std::make_shared<IntensityMapData>(this->_output);
}