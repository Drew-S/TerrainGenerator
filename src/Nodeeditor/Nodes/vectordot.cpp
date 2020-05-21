#include "vectordot.h"

#include <QDoubleSpinBox>
#include <QDebug>

// Create Vector Dot Product Node
ConverterVectorDotNode::ConverterVectorDotNode()
{
    qDebug("Creating Vector Dot Product Node, attaching listeners and UI widget");
    // Setup ui
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();

    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);

    // Attach vector 1 in embedded ui listeners
    QObject::connect(this->_ui.spin_x_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::x0Changed);
    QObject::connect(this->_ui.spin_y_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::y0Changed);
    QObject::connect(this->_ui.spin_z_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::z0Changed);
    QObject::connect(this->_ui.spin_w_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::w0Changed);

    // Attach vector 2 in embedded ui listeners
    QObject::connect(this->_ui.spin_x_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::x1Changed);
    QObject::connect(this->_ui.spin_y_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::y1Changed);
    QObject::connect(this->_ui.spin_z_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::z1Changed);
    QObject::connect(this->_ui.spin_w_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::w1Changed);

    // Attach vector 1 in shared ui listeners
    QObject::connect(this->_shared_ui.spin_x_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::x0Changed);
    QObject::connect(this->_shared_ui.spin_y_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::y0Changed);
    QObject::connect(this->_shared_ui.spin_z_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::z0Changed);
    QObject::connect(this->_shared_ui.spin_w_0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::w0Changed);

    // Attach vector 2 in shared ui listeners
    QObject::connect(this->_shared_ui.spin_x_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::x1Changed);
    QObject::connect(this->_shared_ui.spin_y_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::y1Changed);
    QObject::connect(this->_shared_ui.spin_z_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::z1Changed);
    QObject::connect(this->_shared_ui.spin_w_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterVectorDotNode::w1Changed);
}
ConverterVectorDotNode::~ConverterVectorDotNode() {}

// Caption of the node in the embedded node widget
QString ConverterVectorDotNode::caption() const
{
    return QString("Vector Dot Product");
}

// Name of the widget shown in the selection list
QString ConverterVectorDotNode::name() const
{
    return QString("Vector Dot Product");
}

// The embedded and shared interaction widget
QWidget *ConverterVectorDotNode::embeddedWidget()
{
    return this->_widget;
}
QWidget *ConverterVectorDotNode::sharedWidget()
{
    return this->_shared_widget;
}

// The number of ports (2 input and 1 output)
unsigned int ConverterVectorDotNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 2 : 1;
}

// Get the ports data type, VectorMap in and IntensityMap out
QtNodes::NodeDataType ConverterVectorDotNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    (void)port_index;
    return port_type == QtNodes::PortType::In ? VectorMapData().type() : IntensityMapData().type();
}

// Get the resulting data from the output port
std::shared_ptr<QtNodes::NodeData> ConverterVectorDotNode::outData(QtNodes::PortIndex port)
{
    (void)port;
    return std::make_shared<IntensityMapData>(this->_pixmap);
}

// Save the internal settings for file saving
QJsonObject ConverterVectorDotNode::save() const
{
    qDebug("Saving vector dot product node");
    QJsonObject data;
    data["name"] = this->name();

    QJsonObject val0;
    val0["x"] = this->_in_val_0.x;
    val0["y"] = this->_in_val_0.y;
    val0["z"] = this->_in_val_0.z;
    val0["w"] = this->_in_val_0.w;

    QJsonObject val1;
    val1["x"] = this->_in_val_1.x;
    val1["y"] = this->_in_val_1.y;
    val1["z"] = this->_in_val_1.z;
    val1["w"] = this->_in_val_1.w;

    data["val0"] = val0;
    data["val1"] = val1;

    return data;
}

// Restores the node from the given save data
void ConverterVectorDotNode::restore(QJsonObject const &data)
{
    qDebug("Restoring vector dot product node");
    this->_in_val_0 = glm::dvec4(
        data["val0"].toObject()["x"].toDouble(1.00),
        data["val0"].toObject()["y"].toDouble(1.00),
        data["val0"].toObject()["z"].toDouble(1.00),
        data["val0"].toObject()["w"].toDouble(1.00));

    this->_in_val_1 = glm::dvec4(
        data["val1"].toObject()["x"].toDouble(1.00),
        data["val1"].toObject()["y"].toDouble(1.00),
        data["val1"].toObject()["z"].toDouble(1.00),
        data["val1"].toObject()["w"].toDouble(1.00));

    this->_generate();
}

// Set the input pointer data
void ConverterVectorDotNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port_index)
{
    if (node_data)
    {
        switch ((int)port_index)
        {
        case 0:
            this->_in_0 = std::dynamic_pointer_cast<VectorMapData>(node_data);
            this->_in_set_0 = true;
            this->_ui.spin_x_0->setReadOnly(true);
            this->_ui.spin_y_0->setReadOnly(true);
            this->_ui.spin_z_0->setReadOnly(true);
            this->_ui.spin_w_0->setReadOnly(true);
            break;
        case 1:
            this->_in_1 = std::dynamic_pointer_cast<VectorMapData>(node_data);
            this->_in_set_1 = true;
            this->_ui.spin_x_1->setReadOnly(true);
            this->_ui.spin_y_1->setReadOnly(true);
            this->_ui.spin_z_1->setReadOnly(true);
            this->_ui.spin_w_1->setReadOnly(true);
            break;
        }
        this->_generate();
    }
}

// When an input is deleted allow for using the input values again
void ConverterVectorDotNode::inputConnectionDeleted(QtNodes::Connection const &connection)
{
    switch ((int)connection.getPortIndex(QtNodes::PortType::In))
    {
    case 0:
        this->_in_set_0 = false;
        this->_ui.spin_x_0->setReadOnly(false);
        this->_ui.spin_y_0->setReadOnly(false);
        this->_ui.spin_z_0->setReadOnly(false);
        this->_ui.spin_w_0->setReadOnly(false);
        break;
    case 1:
        this->_in_set_1 = false;
        this->_ui.spin_x_1->setReadOnly(false);
        this->_ui.spin_y_1->setReadOnly(false);
        this->_ui.spin_z_1->setReadOnly(false);
        this->_ui.spin_w_1->setReadOnly(false);
        break;
    }
    this->_generate();
}

// When a vector value is changed, update values and generate output
// First vector
void ConverterVectorDotNode::x0Changed(double value)
{
    this->_in_val_0.x = value;
    this->_ui.spin_x_0->setValue(this->_in_val_0.x);
    this->_shared_ui.spin_x_0->setValue(this->_in_val_0.x);
    this->_generate();
}
void ConverterVectorDotNode::y0Changed(double value)
{
    this->_in_val_0.y = value;
    this->_ui.spin_y_0->setValue(this->_in_val_0.y);
    this->_shared_ui.spin_y_0->setValue(this->_in_val_0.y);
    this->_generate();
}
void ConverterVectorDotNode::z0Changed(double value)
{
    this->_in_val_0.z = value;
    this->_ui.spin_z_0->setValue(this->_in_val_0.z);
    this->_shared_ui.spin_z_0->setValue(this->_in_val_0.z);
    this->_generate();
}
void ConverterVectorDotNode::w0Changed(double value)
{
    this->_in_val_0.w = value;
    this->_ui.spin_w_0->setValue(this->_in_val_0.w);
    this->_shared_ui.spin_w_0->setValue(this->_in_val_0.w);
    this->_generate();
}

// Second vector
void ConverterVectorDotNode::x1Changed(double value)
{
    this->_in_val_1.x = value;
    this->_ui.spin_x_1->setValue(this->_in_val_1.x);
    this->_shared_ui.spin_x_1->setValue(this->_in_val_1.x);
    this->_generate();
}
void ConverterVectorDotNode::y1Changed(double value)
{
    this->_in_val_1.y = value;
    this->_ui.spin_y_1->setValue(this->_in_val_1.y);
    this->_shared_ui.spin_y_1->setValue(this->_in_val_1.y);
    this->_generate();
}
void ConverterVectorDotNode::z1Changed(double value)
{
    this->_in_val_1.z = value;
    this->_ui.spin_z_1->setValue(this->_in_val_1.z);
    this->_shared_ui.spin_z_1->setValue(this->_in_val_1.z);
    this->_generate();
}
void ConverterVectorDotNode::w1Changed(double value)
{
    this->_in_val_1.w = value;
    this->_ui.spin_w_1->setValue(this->_in_val_1.w);
    this->_shared_ui.spin_w_1->setValue(this->_in_val_1.w);
    this->_generate();
}

// Generate the output value
void ConverterVectorDotNode::_generate()
{
    qDebug("Applying tranformation, generating output");
    if (this->_in_set_0 && this->_in_set_1)
        this->_generateInBoth();

    else if (this->_in_set_0 && !this->_in_set_1)
        this->_generateIn1();

    else if (!this->_in_set_0 && this->_in_set_1)
        this->_generateIn1(true);

    else
        this->_generateIn();

    emit this->dataUpdated(0);
}

// Generate when both inputs have maps
void ConverterVectorDotNode::_generateInBoth()
{
    VectorMap map0 = this->_in_0->vectorMap();
    VectorMap map1 = this->_in_1->vectorMap();

    this->_pixmap = IntensityMap(map0.width, map0.height);

    for (int y = 0; y < map0.height; y++)
        for (int x = 0; x < map0.width; x++)
            this->_pixmap.append(ConverterVectorDotNode::dot(map0.at(x, y), map1.at(x, y)));
}

// Generates when only one input is set (use a constant vector for other)
void ConverterVectorDotNode::_generateIn1(bool second)
{
    VectorMap map;
    glm::dvec4 val;
    if (second)
    {
        map = this->_in_1->vectorMap();
        val = this->_in_val_0;
    }
    else
    {
        map = this->_in_0->vectorMap();
        val = this->_in_val_1;
    }

    this->_pixmap = IntensityMap(map.width, map.height);

    for (int y = 0; y < map.height; y++)
        for (int x = 0; x < map.width; x++)
            this->_pixmap.append(ConverterVectorDotNode::dot(map.at(x, y), val));
}

// Generates when neither input is set, use two constants to create a constant out
void ConverterVectorDotNode::_generateIn()
{
    this->_pixmap = IntensityMap(128, 128, ConverterVectorDotNode::dot(this->_in_val_0, this->_in_val_1));
}

// Apply the dot algorithm to both vectors
double ConverterVectorDotNode::dot(glm::dvec4 a, glm::dvec4 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}