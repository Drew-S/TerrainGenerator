#include "constantvalue.h"

#include "Globals/settings.h"

// Node for creating constant value for other nodes
InputConstantValueNode::InputConstantValueNode()
{
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();

    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);

    qDebug("Created constant value node");
}

InputConstantValueNode::~InputConstantValueNode() {}

// Attach UI listeners
void InputConstantValueNode::created()
{
    QObject::connect(this->_ui.constant, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_value = value;
        this->_shared_ui.constant->setValue(this->_value);
        emit this->dataUpdated(0);
    });
    QObject::connect(this->_shared_ui.constant, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_value = value;
        this->_ui.constant->setValue(this->_value);
        emit this->dataUpdated(0);
    });
}

// The text that appears in the node
QString InputConstantValueNode::caption() const
{
    return QString("Constant Value");
}

// The text that appears in the list
QString InputConstantValueNode::name() const
{
    return QString("Constant Value");
}

// The embedded control widget
QWidget *InputConstantValueNode::embeddedWidget()
{
    Q_CHECK_PTR(this->_widget);
    return this->_widget;
}

// The shared control widget
QWidget *InputConstantValueNode::sharedWidget()
{
    Q_CHECK_PTR(this->_shared_widget);
    return this->_shared_widget;
}

// The number of ports, 1 output no input
unsigned int InputConstantValueNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::Out ? 1 : 0;
}

// The data type of the port, intensity map
QtNodes::NodeDataType InputConstantValueNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    return IntensityMapData().type();
}

// Save the node data
QJsonObject InputConstantValueNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    data["value"] = this->_value;
    return data;
}

// Restore the node data
void InputConstantValueNode::restore(QJsonObject const &data)
{
    this->_value = data["value"].toDouble(0.00);
}

// Return the output data
std::shared_ptr<QtNodes::NodeData> InputConstantValueNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    Q_CHECK_PTR(SETTINGS);
    int size = SETTINGS->previewResolution();
    if (SETTINGS->renderMode())
        size = SETTINGS->renderResolution();
    return std::make_shared<IntensityMapData>(IntensityMap(size, size, this->_value));
}

// Set the input data, does nothing
void InputConstantValueNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
{
    Q_UNUSED(node_data);
    Q_UNUSED(port);
}