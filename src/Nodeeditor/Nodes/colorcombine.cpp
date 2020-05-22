#include "colorcombine.h"

#include <QColor>
#include <QDoubleSpinBox>
#include <QDebug>

// TODO: Long term, fork nodeeditor to include better stylings
ConverterColorCombineNode::ConverterColorCombineNode()
{
    qDebug("Created new Color combine node, attaching listeners and UI widget");
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();
    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);

    QObject::connect(this->_ui.spin_red, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterColorCombineNode::redChanged);
    QObject::connect(this->_ui.spin_green, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterColorCombineNode::greenChanged);
    QObject::connect(this->_ui.spin_blue, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterColorCombineNode::blueChanged);
    QObject::connect(this->_ui.spin_alpha, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterColorCombineNode::alphaChanged);

    QObject::connect(this->_shared_ui.spin_red, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterColorCombineNode::redChanged);
    QObject::connect(this->_shared_ui.spin_green, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterColorCombineNode::greenChanged);
    QObject::connect(this->_shared_ui.spin_blue, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterColorCombineNode::blueChanged);
    QObject::connect(this->_shared_ui.spin_alpha, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ConverterColorCombineNode::alphaChanged);
}

ConverterColorCombineNode::~ConverterColorCombineNode() {}

// Title shown at the top of the node
QString ConverterColorCombineNode::caption() const
{
    return QString("Combine Color Channels");
}

// Title shown in the selection list
QString ConverterColorCombineNode::name() const
{
    return QString("Colour Combine");
}

// The embedded control widget
QWidget *ConverterColorCombineNode::embeddedWidget()
{
    Q_CHECK_PTR(this->_widget);
    return this->_widget;
}
QWidget *ConverterColorCombineNode::sharedWidget()
{
    Q_CHECK_PTR(this->_shared_widget);
    return this->_shared_widget;
}

// Get the number of ports (1 output, 4 input)
unsigned int ConverterColorCombineNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 4 : 1;
}

// Get the port datatype
QtNodes::NodeDataType ConverterColorCombineNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    if (port_type == QtNodes::PortType::Out)
        return VectorMapData().type();

    QtNodes::NodeDataType type = IntensityMapData().type();

    switch ((int)port_index)
    {
    case 0:
        return {type.id, "red"};
        break;
    case 1:
        return {type.id, "green"};
        break;
    case 2:
        return {type.id, "blue"};
        break;
    case 3:
        return {type.id, "alpha"};
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
}

// Get the output data (the VectorMapData)
std::shared_ptr<QtNodes::NodeData> ConverterColorCombineNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return std::make_shared<VectorMapData>(this->_pixmap);
}

// Set the input node
void ConverterColorCombineNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port_index)
{
    if (node_data)
    {
        switch ((int)port_index)
        {
        case 0:
            this->_red = std::dynamic_pointer_cast<IntensityMapData>(node_data);
            this->_red_set = true;
            this->_ui.spin_red->setReadOnly(true);
            this->_shared_ui.spin_red->setReadOnly(true);
            break;
        case 1:
            this->_green = std::dynamic_pointer_cast<IntensityMapData>(node_data);
            this->_green_set = true;
            this->_ui.spin_green->setReadOnly(true);
            this->_shared_ui.spin_green->setReadOnly(true);
            break;
        case 2:
            this->_blue = std::dynamic_pointer_cast<IntensityMapData>(node_data);
            this->_blue_set = true;
            this->_ui.spin_blue->setReadOnly(true);
            this->_shared_ui.spin_blue->setReadOnly(true);
            break;
        case 3:
            this->_alpha = std::dynamic_pointer_cast<IntensityMapData>(node_data);
            this->_alpha_set = true;
            this->_ui.spin_alpha->setReadOnly(true);
            this->_shared_ui.spin_alpha->setReadOnly(true);
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
        this->_generate();
    }
}

// Save and load the node
QJsonObject ConverterColorCombineNode::save() const
{
    qDebug("Saving combine node");
    QJsonObject data;

    data["name"] = this->name();
    data["red"] = this->_red_val;
    data["green"] = this->_green_val;
    data["blue"] = this->_blue_val;
    data["alpha"] = this->_alpha_val;

    return data;
}

void ConverterColorCombineNode::restore(QJsonObject const &data)
{
    qDebug("Restoring combine node");
    this->_red_val = data["red"].toDouble(1.00);
    this->_green_val = data["green"].toDouble(1.00);
    this->_blue_val = data["blue"].toDouble(1.00);
    this->_alpha_val = data["alpha"].toDouble(1.00);
}

// On input being deleted update data
void ConverterColorCombineNode::inputConnectionDeleted(QtNodes::Connection const &connection)
{
    switch ((int)connection.getPortIndex(QtNodes::PortType::In))
    {
    case 0:
        this->_red_set = false;
        this->_ui.spin_red->setReadOnly(false);
        this->_shared_ui.spin_red->setReadOnly(false);
        break;
    case 1:
        this->_green_set = false;
        this->_ui.spin_green->setReadOnly(false);
        this->_shared_ui.spin_green->setReadOnly(false);
        break;
    case 2:
        this->_blue_set = false;
        this->_ui.spin_blue->setReadOnly(false);
        this->_shared_ui.spin_blue->setReadOnly(false);
        break;
    case 3:
        this->_alpha_set = false;
        this->_ui.spin_alpha->setReadOnly(false);
        this->_shared_ui.spin_alpha->setReadOnly(false);
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
    this->_generate();
}

// When ui changes update
void ConverterColorCombineNode::redChanged(double value)
{
    this->_red_val = value;
    this->_ui.spin_red->setValue(this->_red_val);
    this->_shared_ui.spin_red->setValue(this->_red_val);
    this->_generate();
}
void ConverterColorCombineNode::greenChanged(double value)
{
    this->_green_val = value;
    this->_ui.spin_green->setValue(this->_green_val);
    this->_shared_ui.spin_green->setValue(this->_green_val);
    this->_generate();
}
void ConverterColorCombineNode::blueChanged(double value)
{
    this->_blue_val = value;
    this->_ui.spin_blue->setValue(this->_blue_val);
    this->_shared_ui.spin_blue->setValue(this->_blue_val);
    this->_generate();
}
void ConverterColorCombineNode::alphaChanged(double value)
{
    this->_alpha_val = value;
    this->_ui.spin_alpha->setValue(this->_alpha_val);
    this->_shared_ui.spin_alpha->setValue(this->_alpha_val);
    this->_generate();
}

// Generates the output
void ConverterColorCombineNode::_generate()
{
    qDebug("Combinging values into output");
    if (!this->_red_set && !this->_green_set && !this->_blue_set && !this->_alpha_set)
    {
        this->_pixmap = VectorMap(1, 1, glm::dvec4(this->_red_val, this->_green_val, this->_blue_val, this->_alpha_val));
    }
    else
    {
        int width = 128;
        int height = 128;
        IntensityMap red(1, 1, this->_red_val);
        IntensityMap green(1, 1, this->_green_val);
        IntensityMap blue(1, 1, this->_blue_val);
        IntensityMap alpha(1, 1, this->_alpha_val);

        if (this->_red_set)
        {
            Q_CHECK_PTR(this->_red);
            red = this->_red->intensityMap();
            width = red.width;
            height = red.height;
        }

        if (this->_green_set)
        {
            Q_CHECK_PTR(this->_green);
            green = this->_green->intensityMap();
            if (width == 128)
            {
                width = green.width;
                height = green.height;
            }
        }

        if (this->_blue_set)
        {
            Q_CHECK_PTR(this->_blue);
            blue = this->_blue->intensityMap();
            if (width == 128)
            {
                width = blue.width;
                height = blue.height;
            }
        }

        if (this->_alpha_set)
        {
            Q_CHECK_PTR(this->_alpha);
            alpha = this->_alpha->intensityMap();
            if (width == 128)
            {
                width = alpha.width;
                height = alpha.height;
            }
        }

        // TODO: use preview/render resolutions
        this->_pixmap = VectorMap(width, height);
        for (int y = 0; y < this->_pixmap.height; y++)
            for (int x = 0; x < this->_pixmap.width; x++)
                this->_pixmap.append(glm::dvec4(
                    red.at(x, y),
                    green.at(x, y),
                    blue.at(x, y),
                    alpha.at(x, y)));
    }
    emit this->dataUpdated(0);
}