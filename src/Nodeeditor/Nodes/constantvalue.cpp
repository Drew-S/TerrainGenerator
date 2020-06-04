#include "constantvalue.h"

#include "Globals/settings.h"

/**
 * InputConstantValueNode
 * 
 * Create a new node and create the UI.
 */
InputConstantValueNode::InputConstantValueNode()
{
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();

    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);

    qDebug("Created constant value node");
}

/**
 * created
 * 
 * Function is called when the node is created so it can connect to listeners.
 * 
 * @signals dataUpdated
 */
void InputConstantValueNode::created()
{
    QObject::connect(this->_ui.constant,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        this->_value = value;
        this->_shared_ui.constant->setValue(this->_value);
        emit this->dataUpdated(0);
    });
    QObject::connect(this->_shared_ui.constant,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        this->_value = value;
        this->_ui.constant->setValue(this->_value);
        emit this->dataUpdated(0);
    });
}

/**
 * caption
 * 
 * Return a string that is displayed on the node and in the properties.
 * 
 * @returns QString : The caption.
 */
QString InputConstantValueNode::caption() const
{
    return QString("Constant Value");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString InputConstantValueNode::name() const
{
    return QString("Constant Value");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram.
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *InputConstantValueNode::embeddedWidget()
{
    Q_CHECK_PTR(this->_widget);
    return this->_widget;
}

/**
 * sharedWidget
 * 
 * Returns a pointer to the widget that gets displayed in the properties panel.
 * 
 * @returns QWidget* : The shared widget.
 */
QWidget *InputConstantValueNode::sharedWidget()
{
    Q_CHECK_PTR(this->_shared_widget);
    return this->_shared_widget;
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
unsigned int InputConstantValueNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::Out ? 1 : 0;
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
InputConstantValueNode::dataType(QtNodes::PortType port_type,
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
QJsonObject InputConstantValueNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    data["value"] = this->_value;
    return data;
}

/**
 * restore
 * 
 * Restores the state of the node from a provided json object.
 * 
 * @param QJsonObject const& data : The data to restore from.
 */
void InputConstantValueNode::restore(QJsonObject const &data)
{
    this->_value = data["value"].toDouble(0.00);
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
InputConstantValueNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    Q_CHECK_PTR(SETTINGS);
    int size = SETTINGS->previewResolution();
    if (SETTINGS->renderMode())
        size = SETTINGS->renderResolution();

    return std::make_shared<IntensityMapData>(IntensityMap(size,
                                                           size,
                                                           this->_value));
}

/**
 * setInData
 * 
 * Sets the input data on a port. (does nothing)
 * 
 * @param std::shared_ptr<QtNodes::NodeData> node_data : The shared pointer data
 *                                                       being inputted.
 * @param QtNodes::PortIndex port : The port the data is being set on.
 */
void InputConstantValueNode::setInData(
    std::shared_ptr<QtNodes::NodeData> node_data,
    QtNodes::PortIndex port)
{
    Q_UNUSED(node_data);
    Q_UNUSED(port);
}