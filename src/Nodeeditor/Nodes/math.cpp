#include "math.h"

#include <math.h> // pow

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDebug>

// Create a ConverterMathNode
ConverterMathNode::ConverterMathNode()
{
    qDebug("Creating Math Node, attaching listeners and UI widget");
    // Ui setup
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();
    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);

    // Ui connections
    QObject::connect(this->_ui.mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConverterMathNode::comboChanged);
    QObject::connect(this->_ui.val_in_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterMathNode::val0Changed);
    QObject::connect(this->_ui.val_in_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterMathNode::val1Changed);

    QObject::connect(this->_shared_ui.mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConverterMathNode::comboChanged);
    QObject::connect(this->_shared_ui.val_in_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterMathNode::val0Changed);
    QObject::connect(this->_shared_ui.val_in_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterMathNode::val1Changed);

    // Generate default result
    this->_generate();
}

ConverterMathNode::~ConverterMathNode() {}

// Title shown at the top of the node
QString ConverterMathNode::caption() const
{
    return QString("Math");
}

// Title shown in the selection list
QString ConverterMathNode::name() const
{
    return QString("ConverterMathNode");
}

void ConverterMathNode::name(QString name)
{
    (void)name;
}

// The widget that is embedded into the node
QWidget *ConverterMathNode::embeddedWidget()
{
    return this->_widget;
}

// The widget that is displayed in the properties panel
QWidget *ConverterMathNode::sharedWidget()
{
    return this->_shared_widget;
}

// Needed for NodeDataModel, not sure where it is used
QString ConverterMathNode::modelName()
{
    return QString("Math Node");
}

// Get the number of in and out ports
unsigned int ConverterMathNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 2 : 1;
}

// Get the input/output data type
QtNodes::NodeDataType ConverterMathNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    (void)port_type;
    (void)port_index;
    return IntensityMapData().type();
}

// Set the input data for a port
void ConverterMathNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
{
    if (node_data)
    {
        if (port == 0)
        {
            this->_in_0 = std::dynamic_pointer_cast<IntensityMapData>(node_data);
            this->_in_0_set = true;
            this->_ui.val_in_0->setReadOnly(true);
        }

        else if (port == 1)
        {
            this->_in_1 = std::dynamic_pointer_cast<IntensityMapData>(node_data);
            this->_in_1_set = true;
            this->_ui.val_in_1->setReadOnly(true);
        }

        this->_generate();
    }
}

// Generate the output intensity map
void ConverterMathNode::_generate()
{
    qDebug("Applying transformation, generating output");
    if (this->_in_0_set && !this->_in_1_set)
        this->_generateIn1(false);

    else if (!this->_in_0_set && this->_in_1_set)
        this->_generateIn1(true);

    else if (this->_in_0_set && this->_in_1_set)
        this->_generateInBoth();

    else
        this->_generateIn();

    emit this->dataUpdated(0);
}

// Generates intensity map by applying transform (pixel by pixel) from one map to the other
// IntensityMap A, B, C
// C = A (op) B
void ConverterMathNode::_generateInBoth()
{
    IntensityMap map_0 = this->_in_0->intensityMap();
    IntensityMap map_1 = this->_in_1->intensityMap();
    switch (this->_mode)
    {
    case ConverterMathNode::MIX:
        this->_pixmap = map_0.transform(&ConverterMathNode::mix, &map_1);
        break;
    case ConverterMathNode::ADD:
        this->_pixmap = map_0.transform(&ConverterMathNode::add, &map_1);
        break;
    case ConverterMathNode::SUBTRACT:
        this->_pixmap = map_0.transform(&ConverterMathNode::subtract, &map_1);
        break;
    case ConverterMathNode::MULTIPLY:
        this->_pixmap = map_0.transform(&ConverterMathNode::multiply, &map_1);
        break;
    case ConverterMathNode::DIVIDE:
        this->_pixmap = map_0.transform(&ConverterMathNode::divide, &map_1);
        break;
    case ConverterMathNode::MIN:
        this->_pixmap = map_0.transform(&ConverterMathNode::min, &map_1);
        break;
    case ConverterMathNode::MAX:
        this->_pixmap = map_0.transform(&ConverterMathNode::max, &map_1);
        break;
    case ConverterMathNode::POW:
        this->_pixmap = map_0.transform(&ConverterMathNode::pow, &map_1);
        break;
    default:
        break;
    }
}
// Generates intensity map by applying transform (pixel by pixel) from one map to a value
// IntensityMap A, C
// double B
// C = A (op) B
void ConverterMathNode::_generateIn1(bool second)
{
    IntensityMap map;
    double val;
    if (second)
    {
        map = this->_in_1->intensityMap();
        val = this->_val_in_0;
    }
    else
    {
        map = this->_in_0->intensityMap();
        val = this->_val_in_1;
    }

    switch (this->_mode)
    {
    case ConverterMathNode::MIX:
        this->_pixmap = map.transform(&ConverterMathNode::mix, val);
        break;
    case ConverterMathNode::ADD:
        this->_pixmap = map.transform(&ConverterMathNode::add, val);
        break;
    case ConverterMathNode::SUBTRACT:
        this->_pixmap = map.transform(&ConverterMathNode::subtract, val);
        break;
    case ConverterMathNode::MULTIPLY:
        this->_pixmap = map.transform(&ConverterMathNode::multiply, val);
        break;
    case ConverterMathNode::DIVIDE:
        this->_pixmap = map.transform(&ConverterMathNode::divide, val);
        break;
    case ConverterMathNode::MIN:
        this->_pixmap = map.transform(&ConverterMathNode::min, val);
        break;
    case ConverterMathNode::MAX:
        this->_pixmap = map.transform(&ConverterMathNode::max, val);
        break;
    case ConverterMathNode::POW:
        this->_pixmap = map.transform(&ConverterMathNode::pow, val);
        break;
    default:
        break;
    }
}

// Generates intensity map by applying transform (pixel by pixel) from one value to a value
// IntensityMap C
// double A, B
// C = A (op) B
void ConverterMathNode::_generateIn()
{
    // TODO: Use render/preview resolution
    switch (this->_mode)
    {
    case ConverterMathNode::MIX:
        this->_pixmap = IntensityMap(128, 128, mix(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterMathNode::ADD:
        this->_pixmap = IntensityMap(128, 128, add(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterMathNode::SUBTRACT:
        this->_pixmap = IntensityMap(128, 128, subtract(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterMathNode::MULTIPLY:
        this->_pixmap = IntensityMap(128, 128, multiply(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterMathNode::DIVIDE:
        this->_pixmap = IntensityMap(128, 128, divide(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterMathNode::MIN:
        this->_pixmap = IntensityMap(128, 128, min(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterMathNode::MAX:
        this->_pixmap = IntensityMap(128, 128, max(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterMathNode::POW:
        this->_pixmap = IntensityMap(128, 128, pow(this->_val_in_0, this->_val_in_1));
        break;
    default:
        break;
    }
}

// When a connection is deleted default to use the double value instead
void ConverterMathNode::inputConnectionDeleted(QtNodes::Connection const &connection)
{
    int port = (int)connection.getPortIndex(QtNodes::PortType::In);
    if (port == 0)
    {
        this->_in_0_set = false;
        this->_ui.val_in_0->setReadOnly(false);
    }
    else if (port == 1)
    {
        this->_in_1_set = false;
        this->_ui.val_in_1->setReadOnly(false);
    }

    this->_generate();
}

// When the UI updates a value, update local reference and regenerated the results
void ConverterMathNode::val0Changed(double value)
{
    this->_val_in_0 = value;
    this->_ui.val_in_0->setValue(this->_val_in_0);
    this->_shared_ui.val_in_0->setValue(this->_val_in_0);
    this->_generate();
}
void ConverterMathNode::val1Changed(double value)
{
    this->_val_in_1 = value;
    this->_ui.val_in_1->setValue(this->_val_in_1);
    this->_shared_ui.val_in_1->setValue(this->_val_in_1);
    this->_generate();
}

// Change the transform method used
void ConverterMathNode::comboChanged(int index)
{
    this->_mode = (ConverterMathNode::Mode)index;
    this->_ui.mode->setCurrentIndex(this->_mode);
    this->_shared_ui.mode->setCurrentIndex(this->_mode);
}

// Save the node to a object for saving to a file
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

// Restore a node from the json data
void ConverterMathNode::restore(QJsonObject const &data)
{
    qDebug("Restoring math node");
    this->_mode = (ConverterMathNode::Mode)data["mode"].toInt(0);
    this->_val_in_0 = data["value_0"].toDouble(1.00);
    this->_val_in_1 = data["value_1"].toDouble(1.00);
}

// Get the resulting pixmap for output
std::shared_ptr<QtNodes::NodeData> ConverterMathNode::outData(QtNodes::PortIndex port)
{
    (void)port;
    return std::make_shared<IntensityMapData>(this->_pixmap);
}

// Agorithms to use for transformations
double ConverterMathNode::mix(double a, double b)
{
    return (a + b) / 2.00;
}
double ConverterMathNode::add(double a, double b)
{
    return a + b;
}
double ConverterMathNode::subtract(double a, double b)
{
    return a - b;
}
double ConverterMathNode::multiply(double a, double b)
{
    return a * b;
}
double ConverterMathNode::divide(double a, double b)
{
    return b == 0.00 ? 0.00 : a / b;
}
double ConverterMathNode::min(double a, double b)
{
    return a < b ? a : b;
}
double ConverterMathNode::max(double a, double b)
{
    return a > b ? a : b;
}
double ConverterMathNode::pow(double a, double b)
{
    return pow(a, b);
}