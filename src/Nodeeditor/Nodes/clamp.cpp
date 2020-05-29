#include "clamp.h"

#include <math.h>

// Clamp intensity map input between 0 and 1
ConverterClampNode::ConverterClampNode()
{
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();

    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);
}
ConverterClampNode::~ConverterClampNode() {}

// When the node is created attach listeners
void ConverterClampNode::created()
{
    QObject::connect(this->_ui.combo_clamp, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        this->_mode = (ConverterClampNode::Mode)index;
        this->_shared_ui.combo_clamp->setCurrentIndex(index);
        this->_generate();
    });
    QObject::connect(this->_ui.spin_min, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_min = value;
        this->_shared_ui.spin_min->setValue(value);
        this->_generate();
    });
    QObject::connect(this->_ui.spin_max, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_max = value;
        this->_shared_ui.spin_max->setValue(value);
        this->_generate();
    });

    QObject::connect(this->_shared_ui.combo_clamp, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        this->_mode = (ConverterClampNode::Mode)index;
        this->_ui.combo_clamp->setCurrentIndex(index);
        this->_generate();
    });
    QObject::connect(this->_shared_ui.spin_min, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_min = value;
        this->_ui.spin_min->setValue(value);
        this->_generate();
    });
    QObject::connect(this->_shared_ui.spin_max, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_max = value;
        this->_ui.spin_max->setValue(value);
        this->_generate();
    });
}

// Title shown at the top of the node
QString ConverterClampNode::caption() const
{
    return QString("Clamp");
}

// Title shown in the selection list
QString ConverterClampNode::name() const
{
    return QString("Clamp");
}

// The image label that is embedded in the node
QWidget *ConverterClampNode::embeddedWidget()
{
    Q_CHECK_PTR(this->_widget);
    return this->_widget;
}
QWidget *ConverterClampNode::sharedWidget()
{
    Q_CHECK_PTR(this->_shared_widget);
    return this->_shared_widget;
}

// Get the number of ports (1 output, 1 input)
unsigned int ConverterClampNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 3 : 1;
}

// Get the port datatype (only imports IntensityMapData)
QtNodes::NodeDataType ConverterClampNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    if (port_type == QtNodes::PortType::Out)
        return IntensityMapData().type();

    QtNodes::NodeDataType type = IntensityMapData().type();

    switch ((int)port_index)
    {
    case 0:
        return type;
        break;
    case 1:
        return {type.id, "min"};
        break;
    case 2:
        return {type.id, "max"};
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
    Q_UNREACHABLE();
}

// Save and load the node for project files
QJsonObject ConverterClampNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    data["mode"] = (int)this->_mode;
    data["min"] = this->_min;
    data["max"] = this->_max;
    return data;
}
void ConverterClampNode::restore(QJsonObject const &data)
{
    this->_mode = (ConverterClampNode::Mode)data["mode"].toInt(0);
    this->_min = data["min"].toDouble(0.00);
    this->_max = data["max"].toDouble(1.00);
}

// Get the output data
std::shared_ptr<QtNodes::NodeData> ConverterClampNode::outData(QtNodes::PortIndex port)
{
    return std::make_shared<IntensityMapData>(this->_output);
}

// Set the input intensity maps or constant
void ConverterClampNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
{
    if (node_data)
    {
        switch ((int)port)
        {
        case 0:
            this->_input = std::dynamic_pointer_cast<IntensityMapData>(node_data);
            this->_set = true;
            break;
        case 1:
            this->_input_min = std::dynamic_pointer_cast<IntensityMapData>(node_data);
            this->_set_min = true;
            this->_ui.spin_min->setReadOnly(true);
            this->_shared_ui.spin_min->setReadOnly(true);
            break;
        case 2:
            this->_input_max = std::dynamic_pointer_cast<IntensityMapData>(node_data);
            this->_set_max = true;
            this->_ui.spin_max->setReadOnly(true);
            this->_shared_ui.spin_max->setReadOnly(true);
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
        this->_generate();
    }
}

// Truncates values below 0 and above 1 to 0 or 1 respectively
double ConverterClampNode::clamp(double v, double min, double max)
{
    return std::min(max, std::max(min, v));
}
// Clamps values between 0 and 1 using sigmoid, or smoother clamping
double ConverterClampNode::sigmoid(double v, double min, double max)
{
    double ea = exp(v);
    return (ea / (ea + 1.00)) * (max - min) + min;
}

// Reset to use constant values when input removed
void ConverterClampNode::inputConnectionDeleted(QtNodes::Connection const &connection)
{
    int port = (int)connection.getPortIndex(QtNodes::PortType::In);
    switch (port)
    {
    case 0:
        this->_set = false;
        this->_output = IntensityMap(1, 1, 1.00);
        emit this->dataUpdated(0);
        break;
    case 1:
        this->_set_min = false;
        this->_ui.spin_min->setReadOnly(false);
        this->_shared_ui.spin_min->setReadOnly(false);
        this->_generate();
        break;
    case 2:
        this->_set_max = false;
        this->_ui.spin_max->setReadOnly(false);
        this->_shared_ui.spin_max->setReadOnly(false);
        this->_generate();
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
}

// Generate the output pixmap
void ConverterClampNode::_generate()
{
    Q_CHECK_PTR(this->_input);
    IntensityMap map = this->_input->intensityMap();
    this->_output = IntensityMap(map.width, map.height);
    IntensityMap min;
    IntensityMap max;

    if (this->_set_min)
    {
        Q_CHECK_PTR(this->_input_min);
        min = this->_input_min->intensityMap();
    }
    else
    {
        min = IntensityMap(1, 1, this->_min);
    }
    if (this->_set_max)
    {
        Q_CHECK_PTR(this->_input_max);
        max = this->_input_max->intensityMap();
    }
    else
    {
        max = IntensityMap(1, 1, this->_max);
    }

    switch (this->_mode)
    {
    case ConverterClampNode::CLAMP:
        for (int y = 0; y < map.height; y++)
            for (int x = 0; x < map.width; x++)
                this->_output.append(ConverterClampNode::clamp(map.at(x, y), min.at(x, y), max.at(x, y)));
        break;
    case ConverterClampNode::SIGMOID:
        for (int y = 0; y < map.height; y++)
            for (int x = 0; x < map.width; x++)
                this->_output.append(ConverterClampNode::sigmoid(map.at(x, y), min.at(x, y), max.at(x, y)));
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
    emit this->dataUpdated(0);
}