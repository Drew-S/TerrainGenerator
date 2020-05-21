#include "vectormath.h"

#include <QDoubleSpinBox>
#include <QComboBox>

ConverterVectorMathNode::ConverterVectorMathNode()
{
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();

    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);

    // First vector, embedded widget
    QObject::connect(this->_ui.spin_x_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorMathNode::x0Changed);
    QObject::connect(this->_ui.spin_y_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorMathNode::y0Changed);
    QObject::connect(this->_ui.spin_z_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorMathNode::z0Changed);

    // Second vector, embedded widget
    QObject::connect(this->_ui.spin_x_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorMathNode::x1Changed);
    QObject::connect(this->_ui.spin_y_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorMathNode::y1Changed);
    QObject::connect(this->_ui.spin_z_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorMathNode::z1Changed);

    // Embedded mode changed
    QObject::connect(this->_ui.mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConverterVectorMathNode::modeChanged);

    // First vector, shared widget
    QObject::connect(this->_shared_ui.spin_x_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorMathNode::x0Changed);
    QObject::connect(this->_shared_ui.spin_y_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorMathNode::y0Changed);
    QObject::connect(this->_shared_ui.spin_z_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorMathNode::z0Changed);

    // Second vector, shared widget
    QObject::connect(this->_shared_ui.spin_x_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorMathNode::x1Changed);
    QObject::connect(this->_shared_ui.spin_y_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorMathNode::y1Changed);
    QObject::connect(this->_shared_ui.spin_z_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorMathNode::z1Changed);

    // Shared mode changed
    QObject::connect(this->_shared_ui.mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConverterVectorMathNode::modeChanged);
}

ConverterVectorMathNode::~ConverterVectorMathNode() {}

// Title shown at the top of the node
// TODO: Fix captions for all nodes
QString ConverterVectorMathNode::caption() const
{
    return QString("VectorMath");
}

// Title shown in the selection list
// TODO: Fix names for all nodes
QString ConverterVectorMathNode::name() const
{
    return QString("VectorMathNode");
}
void ConverterVectorMathNode::name(QString name)
{
    (void)name;
}

// The embedded and shared widgets
QWidget *ConverterVectorMathNode::embeddedWidget()
{
    return this->_widget;
}
QWidget *ConverterVectorMathNode::sharedWidget()
{
    return this->_shared_widget;
}

// Needed for NodeDataModel, not sure where it is used
QString ConverterVectorMathNode::modelName()
{
    return QString("Vector Math Node");
}

// Get the number of ports (1 output, 2 input)
unsigned int ConverterVectorMathNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 2 : 1;
}

// Get the port datatype (only imports VectorMapData)
QtNodes::NodeDataType ConverterVectorMathNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    (void)port_type;
    (void)port_index;
    return VectorMapData().type();
}

// Save and load the node for project files
QJsonObject ConverterVectorMathNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    return data;
}
void ConverterVectorMathNode::restore(QJsonObject const &data)
{
    (void)data;
}

// Get the output data
std::shared_ptr<QtNodes::NodeData> ConverterVectorMathNode::outData(QtNodes::PortIndex port)
{
    return std::make_shared<VectorMapData>(this->_output);
}

// Set the input intensity maps or constant
void ConverterVectorMathNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
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
        }
        this->_generate();
    }
}

void ConverterVectorMathNode::inputConnectionDeleted(QtNodes::Connection const &connection)
{
    int port = (int)connection.getPortIndex(QtNodes::PortType::In);
    if (port == 0)
    {
        this->_in_0_set = false;
        this->_generate();
    }
    else if (port == 1)
    {
        this->_in_1_set = false;
        this->_generate();
    }
}

void ConverterVectorMathNode::x0Changed(double value)
{
    this->_val_in_0.x = value;
    this->_ui.spin_x_0->setValue(value);
    this->_shared_ui.spin_x_0->setValue(value);
    this->_generate();
}
void ConverterVectorMathNode::y0Changed(double value)
{
    this->_val_in_0.y = value;
    this->_ui.spin_y_0->setValue(value);
    this->_shared_ui.spin_y_0->setValue(value);
    this->_generate();
}
void ConverterVectorMathNode::z0Changed(double value)
{
    this->_val_in_0.z = value;
    this->_ui.spin_z_0->setValue(value);
    this->_shared_ui.spin_z_0->setValue(value);
    this->_generate();
}

void ConverterVectorMathNode::x1Changed(double value)
{
    this->_val_in_1.x = value;
    this->_ui.spin_x_1->setValue(value);
    this->_shared_ui.spin_x_1->setValue(value);
    this->_generate();
}
void ConverterVectorMathNode::y1Changed(double value)
{
    this->_val_in_1.y = value;
    this->_ui.spin_y_1->setValue(value);
    this->_shared_ui.spin_y_1->setValue(value);
    this->_generate();
}
void ConverterVectorMathNode::z1Changed(double value)
{
    this->_val_in_1.z = value;
    this->_ui.spin_z_1->setValue(value);
    this->_shared_ui.spin_z_1->setValue(value);
    this->_generate();
}

void ConverterVectorMathNode::_generate()
{
    if (this->_in_0_set && this->_in_1_set)
        this->_generateInBoth();
    else if (this->_in_0_set && !this->_in_1_set)
        this->_generateIn1();
    else if (!this->_in_0_set && this->_in_1_set)
        this->_generateIn1(true);
    else if (!this->_in_0_set && !this->_in_1_set)
        this->_generateIn();

    emit this->dataUpdated(0);
}

void ConverterVectorMathNode::modeChanged(int index)
{
    this->_mode = (ConverterVectorMathNode::Mode)index;
    this->_generate();
}

void ConverterVectorMathNode::_generateInBoth()
{
    VectorMap map_0 = this->_in_0->vectorMap();
    VectorMap map_1 = this->_in_1->vectorMap();
    switch (this->_mode)
    {
    case ConverterVectorMathNode::ADD:
        this->_output = map_0.transform(&ConverterVectorMathNode::add, &map_1);
        break;
    case ConverterVectorMathNode::SUBTRACT:
        this->_output = map_0.transform(&ConverterVectorMathNode::subtract, &map_1);
        break;
    case ConverterVectorMathNode::MULTIPLY:
        this->_output = map_0.transform(&ConverterVectorMathNode::multiply, &map_1);
        break;
    case ConverterVectorMathNode::DIVIDE:
        this->_output = map_0.transform(&ConverterVectorMathNode::divide, &map_1);
        break;
    case ConverterVectorMathNode::CROSS:
        this->_output = map_0.transform(&ConverterVectorMathNode::cross, &map_1);
        break;
    }
}

void ConverterVectorMathNode::_generateIn1(bool second)
{
    VectorMap map;
    glm::dvec4 val;
    if (second)
    {
        map = this->_in_1->vectorMap();
        val = this->_val_in_0;
    }
    else
    {
        map = this->_in_0->vectorMap();
        val = this->_val_in_1;
    }

    switch (this->_mode)
    {
    case ConverterVectorMathNode::ADD:
        this->_output = map.transform(&ConverterVectorMathNode::add, val);
        break;
    case ConverterVectorMathNode::SUBTRACT:
        this->_output = map.transform(&ConverterVectorMathNode::subtract, val);
        break;
    case ConverterVectorMathNode::MULTIPLY:
        this->_output = map.transform(&ConverterVectorMathNode::multiply, val);
        break;
    case ConverterVectorMathNode::DIVIDE:
        this->_output = map.transform(&ConverterVectorMathNode::divide, val);
        break;
    case ConverterVectorMathNode::CROSS:
        this->_output = map.transform(&ConverterVectorMathNode::cross, val);
        break;
    }
}

void ConverterVectorMathNode::_generateIn()
{

    switch (this->_mode)
    {
    case ConverterVectorMathNode::ADD:
        this->_output = VectorMap(128, 128, ConverterVectorMathNode::add(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterVectorMathNode::SUBTRACT:
        this->_output = VectorMap(128, 128, ConverterVectorMathNode::subtract(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterVectorMathNode::MULTIPLY:
        this->_output = VectorMap(128, 128, ConverterVectorMathNode::multiply(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterVectorMathNode::DIVIDE:
        this->_output = VectorMap(128, 128, ConverterVectorMathNode::divide(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterVectorMathNode::CROSS:
        this->_output = VectorMap(128, 128, ConverterVectorMathNode::cross(this->_val_in_0, this->_val_in_1));
        break;
    }
}

glm::dvec4 ConverterVectorMathNode::add(glm::dvec4 a, glm::dvec4 b)
{
    return a + b;
}

glm::dvec4 ConverterVectorMathNode::subtract(glm::dvec4 a, glm::dvec4 b)
{
    return a - b;
}

glm::dvec4 ConverterVectorMathNode::multiply(glm::dvec4 a, glm::dvec4 b)
{
    return glm::dvec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

glm::dvec4 ConverterVectorMathNode::divide(glm::dvec4 a, glm::dvec4 b)
{
    return glm::dvec4(
        b.x == 0 ? 0.00 : a.x / b.x,
        b.y == 0 ? 0.00 : a.y / b.y,
        b.z == 0 ? 0.00 : a.z / b.z,
        b.w == 0 ? 0.00 : a.w / b.w);
}

// | a b | = a * d - b * c
// | c d |
static inline double det(double a, double b, double c, double d)
{
    return (a * d) - (b * c);
}

// Treats 4 value vectors as 3 value for cross
glm::dvec4 ConverterVectorMathNode::cross(glm::dvec4 a, glm::dvec4 b)
{
    return glm::dvec4(det(a.y, a.z, b.y, b.z), det(a.x, a.z, b.x, b.z), det(a.x, a.y, b.x, b.y), 1.00);
}
