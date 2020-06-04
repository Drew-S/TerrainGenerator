#include "math.h"

#include <math.h>

#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>

#include "Globals/settings.h"

#define Q_BETWEEN(low, v, hi) Q_ASSERT(low <= v && v <= hi)

/**
 * ConverterMathNode
 * 
 * Creates the node and creates the UI.
 */
ConverterMathNode::ConverterMathNode()
{
    qDebug("Creating Math Node, attaching listeners and UI widget");
    // Ui setup
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
void ConverterMathNode::created()
{

    // Ui connections
    QObject::connect(this->_ui.mode,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     this,
                     &ConverterMathNode::comboChanged);

    QObject::connect(this->_ui.val_in_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterMathNode::val0Changed);

    QObject::connect(this->_ui.val_in_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterMathNode::val1Changed);

    QObject::connect(this->_shared_ui.mode,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     this,
                     &ConverterMathNode::comboChanged);

    QObject::connect(this->_shared_ui.val_in_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterMathNode::val0Changed);

    QObject::connect(this->_shared_ui.val_in_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterMathNode::val1Changed);

    QObject::connect(SETTINGS, &Settings::renderModeChanged, [this]() {
        if (!this->_in_0_set && !this->_in_1_set)
            this->_generate();
    });

    // Generate default result
    this->_generate();
}

/**
 * caption
 * 
 * Return a string that is displayed on the node and in the properties.
 * 
 * @returns QString : The caption.
 */
QString ConverterMathNode::caption() const
{
    return QString("Math");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString ConverterMathNode::name() const
{
    return QString("Math");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram.
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *ConverterMathNode::embeddedWidget()
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
QWidget *ConverterMathNode::sharedWidget()
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
unsigned int ConverterMathNode::nPorts(QtNodes::PortType port_type) const
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
ConverterMathNode::dataType(QtNodes::PortType port_type,
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
void ConverterMathNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data,
                                  QtNodes::PortIndex port)
{
    if (node_data)
    {
        switch (port)
        {
        case 0:
            this->_in_0 =
                std::dynamic_pointer_cast<IntensityMapData>(node_data);

            this->_in_0_set = true;
            this->_ui.val_in_0->setReadOnly(true);
            break;
        case 1:
            this->_in_1 =
                std::dynamic_pointer_cast<IntensityMapData>(node_data);

            this->_in_1_set = true;
            this->_ui.val_in_1->setReadOnly(true);
            break;
        default:
            Q_UNREACHABLE();
            break;
        }

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
void ConverterMathNode::_generate()
{
    qDebug("Applying transformation, generating output");
    IntensityMap map_0;
    IntensityMap map_1;
    if (this->_in_0_set)
    {
        Q_CHECK_PTR(this->_in_0);
        map_0 = this->_in_0->intensityMap();
    }
    else
    {
        map_0 = IntensityMap(1, 1, this->_val_in_0);
    }
    if (this->_in_1_set)
    {
        Q_CHECK_PTR(this->_in_1);
        map_1 = this->_in_1->intensityMap();
    }
    else
    {
        map_1 = IntensityMap(1, 1, this->_val_in_1);
    }

    switch (this->_mode)
    {
    case ConverterMathNode::MIX:
        this->_output = map_0.transform(&ConverterMathNode::mix, &map_1);
        break;
    case ConverterMathNode::ADD:
        this->_output = map_0.transform(&ConverterMathNode::add, &map_1);
        break;
    case ConverterMathNode::SUBTRACT:
        this->_output = map_0.transform(&ConverterMathNode::subtract, &map_1);
        break;
    case ConverterMathNode::MULTIPLY:
        this->_output = map_0.transform(&ConverterMathNode::multiply, &map_1);
        break;
    case ConverterMathNode::DIVIDE:
        this->_output = map_0.transform(&ConverterMathNode::divide, &map_1);
        break;
    case ConverterMathNode::MIN:
        this->_output = map_0.transform(&ConverterMathNode::min, &map_1);
        break;
    case ConverterMathNode::MAX:
        this->_output = map_0.transform(&ConverterMathNode::max, &map_1);
        break;
    case ConverterMathNode::POW:
        this->_output = map_0.transform(&ConverterMathNode::pow, &map_1);
        break;
    default:
        Q_UNREACHABLE();
        break;
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
void ConverterMathNode::inputConnectionDeleted(
    QtNodes::Connection const &connection)
{
    int port = (int)connection.getPortIndex(QtNodes::PortType::In);
    switch (port)
    {
    case 0:
        this->_in_0_set = false;
        this->_ui.val_in_0->setReadOnly(false);
        break;
    case 1:
        this->_in_1_set = false;
        this->_ui.val_in_1->setReadOnly(false);
        break;
    default:
        Q_UNREACHABLE();
        break;
    }

    this->_generate();
}

/**
 * val0Changed @slot
 * 
 * When the first override value changes update the data and regenerate the
 * output.
 * 
 * @param double value : The new value.
 */
void ConverterMathNode::val0Changed(double value)
{
    this->_val_in_0 = value;
    this->_ui.val_in_0->setValue(this->_val_in_0);
    this->_shared_ui.val_in_0->setValue(this->_val_in_0);
    this->_generate();
}

/**
 * val1Changed @slot
 * 
 * When the second override value changes update the data and regenerate the
 * output.
 * 
 * @param double value : The new value.
 */
void ConverterMathNode::val1Changed(double value)
{
    this->_val_in_1 = value;
    this->_ui.val_in_1->setValue(this->_val_in_1);
    this->_shared_ui.val_in_1->setValue(this->_val_in_1);
    this->_generate();
}

/**
 * comboChanged @slot
 * 
 * When the mode changes update the data and regenerate the
 * output.
 * 
 * @param int index : The mode to use.
 */
void ConverterMathNode::comboChanged(int index)
{
    Q_BETWEEN(0, index, 7);
    this->_mode = (ConverterMathNode::Mode)index;
    this->_ui.mode->setCurrentIndex(this->_mode);
    this->_shared_ui.mode->setCurrentIndex(this->_mode);
    this->_generate();
}

/**
 * save
 * 
 * Saves the state of the node into a QJsonObject for the system to save to
 * file.
 * 
 * @returns QJsonObject : The saved state of the node.
 */
QJsonObject ConverterMathNode::save() const
{
    qDebug("Saving math node");
    QJsonObject data;
    data["name"] = this->name();
    data["mode"] = (int)this->_mode;
    data["value_0"] = this->_val_in_0;
    data["value_1"] = this->_val_in_1;

    return data;
}

/**
 * restore
 * 
 * Restores the state of the node from a provided json object.
 * 
 * @param QJsonObject const& data : The data to restore from.
 */
void ConverterMathNode::restore(QJsonObject const &data)
{
    qDebug("Restoring math node");
    this->_mode = (ConverterMathNode::Mode)data["mode"].toInt(0);
    this->_val_in_0 = data["value_0"].toDouble(1.00);
    this->_val_in_1 = data["value_1"].toDouble(1.00);
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
ConverterMathNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return std::make_shared<IntensityMapData>(this->_output);
}

/**
 * mix
 * 
 * Averages the two values together.
 * 
 * @param double a : The first value.
 * @param double b : The second value.
 * 
 * @returns double : The resulting value.
 */
double ConverterMathNode::mix(double a, double b)
{
    return (a + b) / 2.00;
}

/**
 * add
 * 
 * Adds the two values together.
 * 
 * @param double a : The first value.
 * @param double b : The second value.
 * 
 * @returns double : The resulting value.
 */
double ConverterMathNode::add(double a, double b)
{
    return a + b;
}

/**
 * subtract
 * 
 * Subtracts the two values together.
 * 
 * @param double a : The first value.
 * @param double b : The second value.
 * 
 * @returns double : The resulting value.
 */
double ConverterMathNode::subtract(double a, double b)
{
    return a - b;
}

/**
 * multiply
 * 
 * Multiplies the two values together.
 * 
 * @param double a : The first value.
 * @param double b : The second value.
 * 
 * @returns double : The resulting value.
 */
double ConverterMathNode::multiply(double a, double b)
{
    return a * b;
}

/**
 * divide
 * 
 * Divides the two values together. If b is 0 (div by zero error) 0 is returned
 * instead.
 * 
 * @param double a : The first value.
 * @param double b : The second value.
 * 
 * @returns double : The resulting value.
 */
double ConverterMathNode::divide(double a, double b)
{
    return b == 0.00 ? 0.00 : a / b;
}

/**
 * min
 * 
 * Returns the smallest of the two values.
 * 
 * @param double a : The first value.
 * @param double b : The second value.
 * 
 * @returns double : The resulting value.
 */
double ConverterMathNode::min(double a, double b)
{
    return a < b ? a : b;
}

/**
 * max
 * 
 * Returns the largest of the two values.
 * 
 * @param double a : The first value.
 * @param double b : The second value.
 * 
 * @returns double : The resulting value.
 */
double ConverterMathNode::max(double a, double b)
{
    return a > b ? a : b;
}

/**
 * pow
 * 
 * Raises a to the power of b.
 * 
 * @param double a : The first value.
 * @param double b : The second value.
 * 
 * @returns double : The resulting value.
 */
double ConverterMathNode::pow(double a, double b)
{
    return pow(a, b);
}