#include "vectormath.h"

#include <QDoubleSpinBox>
#include <QComboBox>
#include <QDebug>

#include "Globals/settings.h"

// Creates the node
ConverterVectorMathNode::ConverterVectorMathNode()
{
    qDebug("Creating Vector Math Node, attaching listeners and UI widget");
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();

    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);
}

ConverterVectorMathNode::~ConverterVectorMathNode() {}

void ConverterVectorMathNode::created()
{
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

// Title shown at the top of the node
QString
ConverterVectorMathNode::caption() const
{
    return QString("Vector Math");
}

// Title shown in the selection list
QString ConverterVectorMathNode::name() const
{
    return QString("Vector Math");
}

// The embedded and shared widgets
QWidget *ConverterVectorMathNode::embeddedWidget()
{
    Q_CHECK_PTR(this->_widget);
    return this->_widget;
}
QWidget *ConverterVectorMathNode::sharedWidget()
{
    Q_CHECK_PTR(this->_shared_widget);
    return this->_shared_widget;
}

// Get the number of ports (1 output, 2 input)
unsigned int ConverterVectorMathNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 2 : 1;
}

// Get the port datatype (only imports VectorMapData)
QtNodes::NodeDataType ConverterVectorMathNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    return VectorMapData().type();
}

// Save and load the node for project files
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

// Get the output data
std::shared_ptr<QtNodes::NodeData> ConverterVectorMathNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
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
        default:
            Q_UNREACHABLE();
            break;
        }
        this->_generate();
    }
}

// When input deleted, use default values
void ConverterVectorMathNode::inputConnectionDeleted(QtNodes::Connection const &connection)
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

// Input handlers
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
void ConverterVectorMathNode::w0Changed(double value)
{
    this->_val_in_0.w = value;
    this->_ui.spin_w_0->setValue(value);
    this->_shared_ui.spin_w_0->setValue(value);
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
void ConverterVectorMathNode::w1Changed(double value)
{
    this->_val_in_1.w = value;
    this->_ui.spin_w_1->setValue(value);
    this->_shared_ui.spin_w_1->setValue(value);
    this->_generate();
}

void ConverterVectorMathNode::modeChanged(int index)
{
    this->_mode = (ConverterVectorMathNode::Mode)index;
    this->_generate();
}

// Generate the output
// TODO: Should be able to colllapse _generate*() into a single function nicely
void ConverterVectorMathNode::_generate()
{
    qDebug("Applying transformation, generating output");
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

// Generates the output when both maps are set
void ConverterVectorMathNode::_generateInBoth()
{
    Q_CHECK_PTR(this->_in_0);
    Q_CHECK_PTR(this->_in_1);
    VectorMap map_0 = this->_in_0->vectorMap();
    VectorMap map_1 = this->_in_1->vectorMap();
    switch (this->_mode)
    {
    case ConverterVectorMathNode::MIX:
        this->_output = map_0.transform(&ConverterVectorMathNode::mix, &map_1);
        break;
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

// Generates the output when one map is set
void ConverterVectorMathNode::_generateIn1(bool second)
{
    VectorMap map;
    glm::dvec4 val;
    if (second)
    {
        Q_CHECK_PTR(this->_in_1);
        map = this->_in_1->vectorMap();
        val = this->_val_in_0;
    }
    else
    {
        Q_CHECK_PTR(this->_in_0);
        map = this->_in_0->vectorMap();
        val = this->_val_in_1;
    }

    switch (this->_mode)
    {
    case ConverterVectorMathNode::MIX:
        this->_output = map.transform(&ConverterVectorMathNode::mix, val);
        break;
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

// Generates the output when neither map is set
void ConverterVectorMathNode::_generateIn()
{
    Q_CHECK_PTR(SETTINGS);
    int size;
    if (SETTINGS->renderMode())
        size = SETTINGS->renderResolution();
    else
        size = SETTINGS->previewResolution();
    switch (this->_mode)
    {
    case ConverterVectorMathNode::MIX:
        this->_output = VectorMap(size, size, ConverterVectorMathNode::mix(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterVectorMathNode::ADD:
        this->_output = VectorMap(size, size, ConverterVectorMathNode::add(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterVectorMathNode::SUBTRACT:
        this->_output = VectorMap(size, size, ConverterVectorMathNode::subtract(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterVectorMathNode::MULTIPLY:
        this->_output = VectorMap(size, size, ConverterVectorMathNode::multiply(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterVectorMathNode::DIVIDE:
        this->_output = VectorMap(size, size, ConverterVectorMathNode::divide(this->_val_in_0, this->_val_in_1));
        break;
    case ConverterVectorMathNode::CROSS:
        this->_output = VectorMap(size, size, ConverterVectorMathNode::cross(this->_val_in_0, this->_val_in_1));
        break;
    }
}

// Algorithms to use to manipulate the functions
glm::dvec4 ConverterVectorMathNode::mix(glm::dvec4 a, glm::dvec4 b)
{
    return (a + b) / 2.00;
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
