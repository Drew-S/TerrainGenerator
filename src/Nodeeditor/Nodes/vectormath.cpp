#include "vectormath.h"

#include <QDoubleSpinBox>
#include <QComboBox>
#include <QDebug>

#include "Globals/settings.h"

/**
 * ConverterMathNode
 * 
 * Creates the node and the UI.
 */
ConverterVectorMathNode::ConverterVectorMathNode()
{
    qDebug("Creating Vector Math Node, attaching listeners and UI widget");
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();

    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);
}

/**
 * created
 * 
 * Function is called when the node is created so it can connect to listeners.
 */
void ConverterVectorMathNode::created()
{
    // First vector, embedded widget
    QObject::connect(this->_ui.spin_x_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorMathNode::x0Changed);

    QObject::connect(this->_ui.spin_y_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorMathNode::y0Changed);

    QObject::connect(this->_ui.spin_z_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorMathNode::z0Changed);

    // Second vector, embedded widget
    QObject::connect(this->_ui.spin_x_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorMathNode::x1Changed);

    QObject::connect(this->_ui.spin_y_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorMathNode::y1Changed);

    QObject::connect(this->_ui.spin_z_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorMathNode::z1Changed);

    // Embedded mode changed
    QObject::connect(this->_ui.mode,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     this,
                     &ConverterVectorMathNode::modeChanged);

    // First vector, shared widget
    QObject::connect(this->_shared_ui.spin_x_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorMathNode::x0Changed);

    QObject::connect(this->_shared_ui.spin_y_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorMathNode::y0Changed);

    QObject::connect(this->_shared_ui.spin_z_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorMathNode::z0Changed);

    // Second vector, shared widget
    QObject::connect(this->_shared_ui.spin_x_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorMathNode::x1Changed);

    QObject::connect(this->_shared_ui.spin_y_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorMathNode::y1Changed);

    QObject::connect(this->_shared_ui.spin_z_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorMathNode::z1Changed);

    // Shared mode changed
    QObject::connect(this->_shared_ui.mode,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     this,
                     &ConverterVectorMathNode::modeChanged);

    Q_CHECK_PTR(SETTINGS);
    QObject::connect(SETTINGS, &Settings::previewResolutionChanged, [this]() {
        Q_CHECK_PTR(SETTINGS);
        if (SETTINGS->renderMode())
            return;
        if (!this->_in_0_set && !this->_in_1_set)
            this->_generate();
    });
    QObject::connect(SETTINGS, &Settings::renderResolutionChanged, [this]() {
        qDebug("Render Resolution Changed");
        Q_CHECK_PTR(SETTINGS);
        if (!SETTINGS->renderMode())
            return;
        if (!this->_in_0_set && !this->_in_1_set)
            this->_generate();
    });
    QObject::connect(SETTINGS, &Settings::renderModeChanged, [this]() {
        Q_CHECK_PTR(SETTINGS);
        if (!this->_in_0_set && !this->_in_1_set)
            this->_generate();
    });
}

/**
 * caption
 * 
 * Return a string that is displayed on the node and in the properties.
 * 
 * @returns QString : The caption.
 */
QString ConverterVectorMathNode::caption() const
{
    return QString("Vector Math");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString ConverterVectorMathNode::name() const
{
    return QString("Vector Math");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram.
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *ConverterVectorMathNode::embeddedWidget()
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
QWidget *ConverterVectorMathNode::sharedWidget()
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
unsigned int ConverterVectorMathNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 2 : 1;
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
ConverterVectorMathNode::dataType(QtNodes::PortType port_type,
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
QJsonObject ConverterVectorMathNode::save() const
{
    qDebug("Saving vector math node");
    QJsonObject data;
    data["name"] = this->name();
    data["mode"] = (int)this->_mode;

    QJsonObject vec0;
    vec0["x"] = this->_val_in_0.x;
    vec0["y"] = this->_val_in_0.y;
    vec0["z"] = this->_val_in_0.z;

    QJsonObject vec1;
    vec1["x"] = this->_val_in_0.x;
    vec1["y"] = this->_val_in_0.y;
    vec1["z"] = this->_val_in_0.z;

    data["vec0"] = vec0;
    data["vec1"] = vec1;

    return data;
}

/**
 * restore
 * 
 * Restores the state of the node from a provided json object.
 * 
 * @param QJsonObject const& data : The data to restore from.
 */
void ConverterVectorMathNode::restore(QJsonObject const &data)
{
    qDebug("Restoring vector math node");
    this->_mode = (ConverterVectorMathNode::Mode)data["mode"].toInt(0);

    this->_val_in_0.x = data["vec0"].toObject()["x"].toDouble(1.00);
    this->_val_in_0.y = data["vec0"].toObject()["y"].toDouble(1.00);
    this->_val_in_0.z = data["vec0"].toObject()["z"].toDouble(1.00);

    this->_val_in_1.x = data["vec1"].toObject()["x"].toDouble(1.00);
    this->_val_in_1.y = data["vec1"].toObject()["y"].toDouble(1.00);
    this->_val_in_1.z = data["vec1"].toObject()["z"].toDouble(1.00);

    this->_generate();
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
ConverterVectorMathNode::outData(QtNodes::PortIndex port)
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
void ConverterVectorMathNode::setInData(
    std::shared_ptr<QtNodes::NodeData> node_data,
    QtNodes::PortIndex port)
{
    if (node_data)
    {
        switch ((int)port)
        {
        case 0:
            this->_in_0 = std::dynamic_pointer_cast<VectorMapData>(node_data);
            this->_in_0_set = true;
            break;
        case 1:
            this->_in_1 = std::dynamic_pointer_cast<VectorMapData>(node_data);
            this->_in_1_set = true;
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
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
 */
void ConverterVectorMathNode::inputConnectionDeleted(
    QtNodes::Connection const &connection)
{
    int port = (int)connection.getPortIndex(QtNodes::PortType::In);
    switch (port)
    {
    case 0:
        this->_in_0_set = false;
        this->_generate();
        break;
    case 1:
        this->_in_1_set = false;
        this->_generate();
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
}

/**
 * x0Changed @slot
 * 
 * Changes the x value on the first vector.
 * 
 * @param double value : The new value.
 */
void ConverterVectorMathNode::x0Changed(double value)
{
    this->_val_in_0.x = value;
    this->_ui.spin_x_0->setValue(value);
    this->_shared_ui.spin_x_0->setValue(value);
    this->_generate();
}

/**
 * y0Changed @slot
 * 
 * Changes the y value on the first vector.
 * 
 * @param double value : The new value.
 */
void ConverterVectorMathNode::y0Changed(double value)
{
    this->_val_in_0.y = value;
    this->_ui.spin_y_0->setValue(value);
    this->_shared_ui.spin_y_0->setValue(value);
    this->_generate();
}

/**
 * z0Changed @slot
 * 
 * Changes the z value on the first vector.
 * 
 * @param double value : The new value.
 */
void ConverterVectorMathNode::z0Changed(double value)
{
    this->_val_in_0.z = value;
    this->_ui.spin_z_0->setValue(value);
    this->_shared_ui.spin_z_0->setValue(value);
    this->_generate();
}

/**
 * w0Changed @slot
 * 
 * Changes the w value on the first vector.
 * 
 * @param double value : The new value.
 */
void ConverterVectorMathNode::w0Changed(double value)
{
    this->_val_in_0.w = value;
    this->_ui.spin_w_0->setValue(value);
    this->_shared_ui.spin_w_0->setValue(value);
    this->_generate();
}

/**
 * x1Changed @slot
 * 
 * Changes the x value on the second vector.
 * 
 * @param double value : The new value.
 */
void ConverterVectorMathNode::x1Changed(double value)
{
    this->_val_in_1.x = value;
    this->_ui.spin_x_1->setValue(value);
    this->_shared_ui.spin_x_1->setValue(value);
    this->_generate();
}

/**
 * y1Changed @slot
 * 
 * Changes the y value on the second vector.
 * 
 * @param double value : The new value.
 */
void ConverterVectorMathNode::y1Changed(double value)
{
    this->_val_in_1.y = value;
    this->_ui.spin_y_1->setValue(value);
    this->_shared_ui.spin_y_1->setValue(value);
    this->_generate();
}

/**
 * z1Changed @slot
 * 
 * Changes the z value on the second vector.
 * 
 * @param double value : The new value.
 */
void ConverterVectorMathNode::z1Changed(double value)
{
    this->_val_in_1.z = value;
    this->_ui.spin_z_1->setValue(value);
    this->_shared_ui.spin_z_1->setValue(value);
    this->_generate();
}

/**
 * w1Changed @slot
 * 
 * Changes the w value on the second vector.
 * 
 * @param double value : The new value.
 */
void ConverterVectorMathNode::w1Changed(double value)
{
    this->_val_in_1.w = value;
    this->_ui.spin_w_1->setValue(value);
    this->_shared_ui.spin_w_1->setValue(value);
    this->_generate();
}

/**
 * modeChanged @slot
 * 
 * Changes the mode that selects what algorithm to apply to the two vectors.
 * 
 * @param int index : The new mode.
 */
void ConverterVectorMathNode::modeChanged(int index)
{
    this->_mode = (ConverterVectorMathNode::Mode)index;
    this->_generate();
}

/**
 * _generate
 * 
 * Generates the output data from the supplied and available data.
 * 
 * @signals dataUpdated
 */
void ConverterVectorMathNode::_generate()
{
    qDebug("Applying transformation, generating output");
    VectorMap map_0;
    VectorMap map_1;
    if (this->_in_0_set)
    {
        Q_CHECK_PTR(this->_in_0);
        map_0 = this->_in_0->vectorMap();
    }
    else
    {
        map_0 = VectorMap(1, 1, this->_val_in_0);
    }
    if (this->_in_1_set)
    {
        Q_CHECK_PTR(this->_in_1);
        map_1 = this->_in_1->vectorMap();
    }
    else
    {
        map_1 = VectorMap(1, 1, this->_val_in_1);
    }

    switch (this->_mode)
    {
    case ConverterVectorMathNode::MIX:
        this->_output = map_0.transform(&ConverterVectorMathNode::mix, &map_1);
        break;
    case ConverterVectorMathNode::ADD:
        this->_output = map_0.transform(&ConverterVectorMathNode::add, &map_1);
        break;
    case ConverterVectorMathNode::SUBTRACT:
        this->_output =
            map_0.transform(&ConverterVectorMathNode::subtract, &map_1);
        break;
    case ConverterVectorMathNode::MULTIPLY:
        this->_output =
            map_0.transform(&ConverterVectorMathNode::multiply, &map_1);
        break;
    case ConverterVectorMathNode::DIVIDE:
        this->_output =
            map_0.transform(&ConverterVectorMathNode::divide, &map_1);
        break;
    case ConverterVectorMathNode::CROSS:
        this->_output =
            map_0.transform(&ConverterVectorMathNode::cross, &map_1);
        break;
    }

    emit this->dataUpdated(0);
}

/**
 * mix
 * 
 * Averages the two vectors together.
 * 
 * @param glm::dvec4 a : The first vector.
 * @param glm::dvec4 b : The second vector.
 * 
 * @returns glm::dvec4 : The resulting vector.
 */
glm::dvec4 ConverterVectorMathNode::mix(glm::dvec4 a, glm::dvec4 b)
{
    return (a + b) / 2.00;
}

/**
 * add
 * 
 * Adds the two vectors together.
 * 
 * @param glm::dvec4 a : The first vector.
 * @param glm::dvec4 b : The second vector.
 * 
 * @returns glm::dvec4 : The resulting vector.
 */
glm::dvec4 ConverterVectorMathNode::add(glm::dvec4 a, glm::dvec4 b)
{
    return a + b;
}

/**
 * subtract
 * 
 * Subtracts the two vectors together.
 * 
 * @param glm::dvec4 a : The first vector.
 * @param glm::dvec4 b : The second vector.
 * 
 * @returns glm::dvec4 : The resulting vector.
 */
glm::dvec4 ConverterVectorMathNode::subtract(glm::dvec4 a, glm::dvec4 b)
{
    return a - b;
}

/**
 * multiply
 * 
 * Multiplies the two vectors together elementwise.
 * 
 * @param glm::dvec4 a : The first vector.
 * @param glm::dvec4 b : The second vector.
 * 
 * @returns glm::dvec4 : The resulting vector.
 */
glm::dvec4 ConverterVectorMathNode::multiply(glm::dvec4 a, glm::dvec4 b)
{
    return glm::dvec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

/**
 * divide
 * 
 * Divides the two vectors together elementwise. If any b values are 0, 0 is
 * uses rather than occuring a divide by zero error.
 * 
 * @param glm::dvec4 a : The first vector.
 * @param glm::dvec4 b : The second vector.
 * 
 * @returns glm::dvec4 : The resulting vector.
 */
glm::dvec4 ConverterVectorMathNode::divide(glm::dvec4 a, glm::dvec4 b)
{
    return glm::dvec4(
        b.x == 0 ? 0.00 : a.x / b.x,
        b.y == 0 ? 0.00 : a.y / b.y,
        b.z == 0 ? 0.00 : a.z / b.z,
        b.w == 0 ? 0.00 : a.w / b.w);
}

/**
 * det
 * 
 * Helper function used by cross below to calculate the determinate of a matrix.
 * 
 * | a b |
 * | c d | = a * d - b * c
 * 
 * @param double a : The first value.
 * @param double b : The second value.
 * @param double c : The third value.
 * @param double d : The fourth value.
 * 
 * @returns double : The result of the determinant.
 */
static inline double det(double a, double b, double c, double d)
{
    return (a * d) - (b * c);
}

/**
 * cross
 * 
 * Uses the cross product on the two vectors. (only considers the first 3 values
 * as cross is only defined for 3 and 7 length vectors, w is replaced by 1.00).
 * 
 * @param glm::dvec4 a : The first vector.
 * @param glm::dvec4 b : The second vector.
 * 
 * @returns glm::dvec4 : The resulting vector.
 */
glm::dvec4 ConverterVectorMathNode::cross(glm::dvec4 a, glm::dvec4 b)
{
    return glm::dvec4(det(a.y, a.z, b.y, b.z), -det(a.x, a.z, b.x, b.z), det(a.x, a.y, b.x, b.y), 1.00);
}
