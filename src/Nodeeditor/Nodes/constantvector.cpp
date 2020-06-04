#include "constantvector.h"

#include <glm/vec4.hpp>

#include "Globals/settings.h"

/**
 * InputConstantVectorNode
 * 
 * Creates a constant vector node and creates the control widgets.
 */
InputConstantVectorNode::InputConstantVectorNode()
{
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();

    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);

    qDebug("Created constant vector node");
}

/**
 * created
 * 
 * Function is called when the node is created so it can connect to listeners.
 * 
 * @signals dataUpdated
 */
void InputConstantVectorNode::created()
{
    // Connect change x to update data
    QObject::connect(this->_ui.spin_x,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        this->_x = value;
        this->_shared_ui.spin_x->setValue(this->_x);
        emit this->dataUpdated(0);
    });

    // Connect change y to update data
    QObject::connect(this->_ui.spin_y,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        this->_y = value;
        this->_shared_ui.spin_y->setValue(this->_y);
        emit this->dataUpdated(0);
    });

    // Connect change z to update data
    QObject::connect(this->_ui.spin_z,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        this->_z = value;
        this->_shared_ui.spin_z->setValue(this->_z);
        emit this->dataUpdated(0);
    });

    // Connect change w to update data
    QObject::connect(this->_ui.spin_w,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        this->_w = value;
        this->_shared_ui.spin_w->setValue(this->_w);
        emit this->dataUpdated(0);
    });


    // Connect change x to update data
    QObject::connect(this->_shared_ui.spin_x,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        this->_x = value;
        this->_ui.spin_x->setValue(this->_x);
        emit this->dataUpdated(0);
    });

    // Connect change y to update data
    QObject::connect(this->_shared_ui.spin_y,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        this->_y = value;
        this->_ui.spin_y->setValue(this->_y);
        emit this->dataUpdated(0);
    });

    // Connect change z to update data
    QObject::connect(this->_shared_ui.spin_z,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        this->_z = value;
        this->_ui.spin_z->setValue(this->_z);
        emit this->dataUpdated(0);
    });


    // Connect change w to update data
    QObject::connect(this->_shared_ui.spin_w,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        this->_w = value;
        this->_ui.spin_w->setValue(this->_w);
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
QString InputConstantVectorNode::caption() const
{
    return QString("Constant Vector");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString InputConstantVectorNode::name() const
{
    return QString("Constant Vector");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram.
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *InputConstantVectorNode::embeddedWidget()
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
QWidget *InputConstantVectorNode::sharedWidget()
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
unsigned int InputConstantVectorNode::nPorts(QtNodes::PortType port_type) const
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
InputConstantVectorNode::dataType(QtNodes::PortType port_type,
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

/**
 * restore
 * 
 * Restores the state of the node from a provided json object.
 * 
 * @param QJsonObject const& data : The data to restore from.
 */
void InputConstantVectorNode::restore(QJsonObject const &data)
{
    this->_x = data["x"].toDouble(0.00);
    this->_y = data["y"].toDouble(0.00);
    this->_z = data["z"].toDouble(0.00);
    this->_w = data["w"].toDouble(0.00);
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
InputConstantVectorNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    Q_CHECK_PTR(SETTINGS);
    int size = SETTINGS->previewResolution();
    if (SETTINGS->renderMode())
        size = SETTINGS->renderResolution();

    return std::make_shared<VectorMapData>(VectorMap(size,
                                                     size,
                                                     glm::dvec4(this->_x,
                                                                this->_y,
                                                                this->_z,
                                                                this->_w)));
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
void InputConstantVectorNode::setInData(
    std::shared_ptr<QtNodes::NodeData> node_data,
    QtNodes::PortIndex port)
{
    Q_UNUSED(node_data);
    Q_UNUSED(port);
}