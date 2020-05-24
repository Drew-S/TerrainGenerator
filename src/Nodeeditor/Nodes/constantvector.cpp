#include "constantvector.h"

#include "Globals/settings.h"

#include <glm/vec4.hpp>

// Node for creating constant value for other nodes
InputConstantVectorNode::InputConstantVectorNode()
{
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();

    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);

    qDebug("Created constant vector node");
}

InputConstantVectorNode::~InputConstantVectorNode() {}

// Attach UI listeners
void InputConstantVectorNode::created()
{
    QObject::connect(this->_ui.spin_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_x = value;
        this->_shared_ui.spin_x->setValue(this->_x);
        emit this->dataUpdated(0);
    });
    QObject::connect(this->_ui.spin_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_y = value;
        this->_shared_ui.spin_y->setValue(this->_y);
        emit this->dataUpdated(0);
    });
    QObject::connect(this->_ui.spin_z, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_z = value;
        this->_shared_ui.spin_z->setValue(this->_z);
        emit this->dataUpdated(0);
    });
    QObject::connect(this->_ui.spin_w, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_w = value;
        this->_shared_ui.spin_w->setValue(this->_w);
        emit this->dataUpdated(0);
    });

    QObject::connect(this->_shared_ui.spin_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_x = value;
        this->_ui.spin_x->setValue(this->_x);
        emit this->dataUpdated(0);
    });
    QObject::connect(this->_shared_ui.spin_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_y = value;
        this->_ui.spin_y->setValue(this->_y);
        emit this->dataUpdated(0);
    });
    QObject::connect(this->_shared_ui.spin_z, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_z = value;
        this->_ui.spin_z->setValue(this->_z);
        emit this->dataUpdated(0);
    });
    QObject::connect(this->_shared_ui.spin_w, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_w = value;
        this->_ui.spin_w->setValue(this->_w);
        emit this->dataUpdated(0);
    });
}

// The text that appears in the node
QString InputConstantVectorNode::caption() const
{
    return QString("Constant Vector");
}

// The text that appears in the list
QString InputConstantVectorNode::name() const
{
    return QString("Constant Vector");
}

// The embedded control widget
QWidget *InputConstantVectorNode::embeddedWidget()
{
    Q_CHECK_PTR(this->_widget);
    return this->_widget;
}

// The shared control widget
QWidget *InputConstantVectorNode::sharedWidget()
{
    Q_CHECK_PTR(this->_shared_widget);
    return this->_shared_widget;
}

// The number of ports, 1 output no input
unsigned int InputConstantVectorNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::Out ? 1 : 0;
}

// The data type of the port, intensity map
QtNodes::NodeDataType InputConstantVectorNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    return VectorMapData().type();
}

// Save the node data
QJsonObject InputConstantVectorNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    data["x"] = this->_x;
    data["y"] = this->_y;
    data["z"] = this->_z;
    data["w"] = this->_w;
    return data;
}

// Restore the node data
void InputConstantVectorNode::restore(QJsonObject const &data)
{
    this->_x = data["x"].toDouble(0.00);
    this->_y = data["y"].toDouble(0.00);
    this->_z = data["z"].toDouble(0.00);
    this->_w = data["w"].toDouble(0.00);
}

// Return the output data
std::shared_ptr<QtNodes::NodeData> InputConstantVectorNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    Q_CHECK_PTR(SETTINGS);
    int size = SETTINGS->previewResolution();
    if (SETTINGS->renderMode())
        size = SETTINGS->renderResolution();

    return std::make_shared<VectorMapData>(VectorMap(size, size, glm::dvec4(this->_x, this->_y, this->_z, this->_w)));
}

// Set the input data, does nothing
void InputConstantVectorNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
{
    Q_UNUSED(node_data);
    Q_UNUSED(port);
}