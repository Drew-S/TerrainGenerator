#include "mathnode.h"

#include <math.h> // pow

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDebug>

// Create a MathNode
MathNode::MathNode()
{
    // Ui setup
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();
    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);

    // Ui connections
    QObject::connect(this->_ui.mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MathNode::comboChanged);
    QObject::connect(this->_ui.val_in_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MathNode::val0Changed);
    QObject::connect(this->_ui.val_in_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MathNode::val1Changed);

    QObject::connect(this->_shared_ui.mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MathNode::comboChanged);
    QObject::connect(this->_shared_ui.val_in_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MathNode::val0Changed);
    QObject::connect(this->_shared_ui.val_in_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MathNode::val1Changed);

    // Generate default result
    this->_gen();
}

MathNode::~MathNode() {}

// Title shown at the top of the node
QString MathNode::caption() const
{
    return QString("Math");
}

// Title shown in the selection list
QString MathNode::name() const
{
    return QString("MathNode");
}

void MathNode::name(QString name)
{
    (void)name;
}

// The widget that is embedded into the node
QWidget *MathNode::embeddedWidget()
{
    return this->_widget;
}

// The widget that is displayed in the properties panel
QWidget *MathNode::sharedWidget()
{
    return this->_shared_widget;
}

// Needed for NodeDataModel, not sure where it is used
QString MathNode::modelName()
{
    return QString("Math Node");
}

// Get the number of in and out ports
unsigned int MathNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 2 : 1;
}

// Get the input/output data type
QtNodes::NodeDataType MathNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    (void)port_type;
    (void)port_index;
    return IntensityMapData().type();
}

// Set the input data for a port
void MathNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
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

        this->_gen();
    }
}

// Generate the output intensity map
void MathNode::_gen()
{
    if (this->_in_0_set && !this->_in_1_set)
        this->_gen_in_1(false);

    else if (!this->_in_0_set && this->_in_1_set)
        this->_gen_in_1(true);

    else if (this->_in_0_set && this->_in_1_set)
        this->_gen_in_both();

    else
        this->_gen_in();

    emit this->dataUpdated(0);
}

// Generates intensity map by applying transform (pixel by pixel) from one map to the other
// IntensityMap A, B, C
// C = A (op) B
void MathNode::_gen_in_both()
{
    IntensityMap map_0 = this->_in_0->intensityMap();
    IntensityMap map_1 = this->_in_1->intensityMap();
    switch (this->_mode)
    {
    case MathNode::MIX:
        this->_pixmap = map_0.transform(&MathNode::mix, &map_1);
        break;
    case MathNode::ADD:
        this->_pixmap = map_0.transform(&MathNode::add, &map_1);
        break;
    case MathNode::SUBTRACT:
        this->_pixmap = map_0.transform(&MathNode::subtract, &map_1);
        break;
    case MathNode::MULTIPLY:
        this->_pixmap = map_0.transform(&MathNode::multiply, &map_1);
        break;
    case MathNode::DIVIDE:
        this->_pixmap = map_0.transform(&MathNode::divide, &map_1);
        break;
    case MathNode::MIN:
        this->_pixmap = map_0.transform(&MathNode::min, &map_1);
        break;
    case MathNode::MAX:
        this->_pixmap = map_0.transform(&MathNode::max, &map_1);
        break;
    case MathNode::POW:
        this->_pixmap = map_0.transform(&MathNode::pow, &map_1);
        break;
    default:
        break;
    }
}
// Generates intensity map by applying transform (pixel by pixel) from one map to a value
// IntensityMap A, C
// double B
// C = A (op) B
void MathNode::_gen_in_1(bool second)
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
    case MathNode::MIX:
        this->_pixmap = map.transform(&MathNode::mix, val);
        break;
    case MathNode::ADD:
        this->_pixmap = map.transform(&MathNode::add, val);
        break;
    case MathNode::SUBTRACT:
        this->_pixmap = map.transform(&MathNode::subtract, val);
        break;
    case MathNode::MULTIPLY:
        this->_pixmap = map.transform(&MathNode::multiply, val);
        break;
    case MathNode::DIVIDE:
        this->_pixmap = map.transform(&MathNode::divide, val);
        break;
    case MathNode::MIN:
        this->_pixmap = map.transform(&MathNode::min, val);
        break;
    case MathNode::MAX:
        this->_pixmap = map.transform(&MathNode::max, val);
        break;
    case MathNode::POW:
        this->_pixmap = map.transform(&MathNode::pow, val);
        break;
    default:
        break;
    }
}

// Generates intensity map by applying transform (pixel by pixel) from one value to a value
// IntensityMap C
// double A, B
// C = A (op) B
void MathNode::_gen_in()
{
    // TODO: Use render/preview resolution
    switch (this->_mode)
    {
    case MathNode::MIX:
        this->_pixmap = IntensityMap(128, 128, mix(this->_val_in_0, this->_val_in_1));
        break;
    case MathNode::ADD:
        this->_pixmap = IntensityMap(128, 128, add(this->_val_in_0, this->_val_in_1));
        break;
    case MathNode::SUBTRACT:
        this->_pixmap = IntensityMap(128, 128, subtract(this->_val_in_0, this->_val_in_1));
        break;
    case MathNode::MULTIPLY:
        this->_pixmap = IntensityMap(128, 128, multiply(this->_val_in_0, this->_val_in_1));
        break;
    case MathNode::DIVIDE:
        this->_pixmap = IntensityMap(128, 128, divide(this->_val_in_0, this->_val_in_1));
        break;
    case MathNode::MIN:
        this->_pixmap = IntensityMap(128, 128, min(this->_val_in_0, this->_val_in_1));
        break;
    case MathNode::MAX:
        this->_pixmap = IntensityMap(128, 128, max(this->_val_in_0, this->_val_in_1));
        break;
    case MathNode::POW:
        this->_pixmap = IntensityMap(128, 128, pow(this->_val_in_0, this->_val_in_1));
        break;
    default:
        break;
    }
}

// When a connection is deleted default to use the double value instead
void MathNode::inputConnectionDeleted(QtNodes::Connection const &connection)
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

    this->_gen();
}

// When the UI updates a value, update local reference and regenerated the results
void MathNode::val0Changed(double value)
{
    this->_val_in_0 = value;
    this->_ui.val_in_0->setValue(this->_val_in_0);
    this->_shared_ui.val_in_0->setValue(this->_val_in_0);
    this->_gen();
}
void MathNode::val1Changed(double value)
{
    this->_val_in_1 = value;
    this->_ui.val_in_1->setValue(this->_val_in_1);
    this->_shared_ui.val_in_1->setValue(this->_val_in_1);
    this->_gen();
}

// Change the transform method used
void MathNode::comboChanged(int index)
{
    this->_mode = (MathNode::Mode)index;
    this->_ui.mode->setCurrentIndex(this->_mode);
    this->_shared_ui.mode->setCurrentIndex(this->_mode);
}

// Save the node to a object for saving to a file
QJsonObject MathNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    data["mode"] = this->_mode;
    data["value_0"] = this->_val_in_0;
    data["value_1"] = this->_val_in_1;

    return data;
}

// Restore a node from the json data
void MathNode::restore(QJsonObject const &data)
{
    this->_mode = (MathNode::Mode)data["mode"].toInt();
    this->_val_in_0 = data["value_0"].toDouble(1.00);
    this->_val_in_1 = data["value_1"].toDouble(1.00);
}

// Get the resulting pixmap for output
std::shared_ptr<QtNodes::NodeData> MathNode::outData(QtNodes::PortIndex port)
{
    (void)port;
    return std::make_shared<IntensityMapData>(this->_pixmap);
}

// Agorithms to use for transformations
double MathNode::mix(double a, double b)
{
    return (a + b) / 2.00;
}
double MathNode::add(double a, double b)
{
    return a + b;
}
double MathNode::subtract(double a, double b)
{
    return a - b;
}
double MathNode::multiply(double a, double b)
{
    return a * b;
}
double MathNode::divide(double a, double b)
{
    return b == 0.00 ? 0.00 : a / b;
}
double MathNode::min(double a, double b)
{
    return a < b ? a : b;
}
double MathNode::max(double a, double b)
{
    return a > b ? a : b;
}
double MathNode::pow(double a, double b)
{
    return pow(a, b);
}