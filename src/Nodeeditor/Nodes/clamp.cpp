#include "clamp.h"

#include <math.h>

/**
 * ConverterClampNode
 * 
 * Creates a clamping node attaching the UI.
 */
ConverterClampNode::ConverterClampNode()
{
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
void ConverterClampNode::created()
{
    // Connect combo box to set mode
    QObject::connect(this->_ui.combo_clamp,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     [this](int index)
    {
        this->_mode = (ConverterClampNode::Mode)index;
        this->_shared_ui.combo_clamp->setCurrentIndex(index);
        this->_generate();
    });

    // Connect minimum spin box to minimum value
    QObject::connect(this->_ui.spin_min,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        this->_min = value;
        this->_shared_ui.spin_min->setValue(value);
        this->_generate();
    });

    // Connect maximum spin box to maximum value
    QObject::connect(this->_ui.spin_max,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        this->_max = value;
        this->_shared_ui.spin_max->setValue(value);
        this->_generate();
    });

    // Connect combo box to set mode
    QObject::connect(this->_shared_ui.combo_clamp,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     [this](int index)
    {
        this->_mode = (ConverterClampNode::Mode)index;
        this->_ui.combo_clamp->setCurrentIndex(index);
        this->_generate();
    });

    // Connect minimum spin box to minimum value
    QObject::connect(this->_shared_ui.spin_min,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        this->_min = value;
        this->_ui.spin_min->setValue(value);
        this->_generate();
    });

    // Connect maximum spin box to maximum value
    QObject::connect(this->_shared_ui.spin_max,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        this->_max = value;
        this->_ui.spin_max->setValue(value);
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
QString ConverterClampNode::caption() const
{
    return QString("Clamp");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString ConverterClampNode::name() const
{
    return QString("Clamp");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram.
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *ConverterClampNode::embeddedWidget()
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
QWidget *ConverterClampNode::sharedWidget()
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
unsigned int ConverterClampNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 3 : 1;
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
ConverterClampNode::dataType(QtNodes::PortType port_type,
                             QtNodes::PortIndex port_index) const
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

/**
 * save
 * 
 * Saves the state of the node into a QJsonObject for the system to save to
 * file.
 * 
 * @returns QJsonObject : The saved state of the node.
 */
QJsonObject ConverterClampNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    data["mode"] = (int)this->_mode;
    data["min"] = this->_min;
    data["max"] = this->_max;
    return data;
}

/**
 * restore
 * 
 * Restores the state of the node from a provided json object.
 * 
 * @param QJsonObject const& data : The data to restore from.
 */
void ConverterClampNode::restore(QJsonObject const &data)
{
    this->_mode = (ConverterClampNode::Mode)data["mode"].toInt(0);
    this->_min = data["min"].toDouble(0.00);
    this->_max = data["max"].toDouble(1.00);
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
ConverterClampNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return std::make_shared<IntensityMapData>(this->_output);
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
void ConverterClampNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data,
                                   QtNodes::PortIndex port)
{
    if (node_data)
    {
        switch ((int)port)
        {
        // Set input data
        case 0:
            this->_input =
                std::dynamic_pointer_cast<IntensityMapData>(node_data);

            this->_set = true;
            break;
        // Set minimum data
        case 1:
            this->_input_min =
                std::dynamic_pointer_cast<IntensityMapData>(node_data);

            this->_set_min = true;
            this->_ui.spin_min->setReadOnly(true);
            this->_shared_ui.spin_min->setReadOnly(true);
            break;
        // Set maximum data
        case 2:
            this->_input_max =
                std::dynamic_pointer_cast<IntensityMapData>(node_data);

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

/**
 * clamp
 * 
 * Algorithm to truncate values beyond the min and max value to min or max
 * respectively.
 * 
 * @param double v : The value to be truncated.
 * @param double min : The minimum value.
 * @param double max : The maximum value.
 * 
 * @returns double : The truncated value.
 */
double ConverterClampNode::clamp(double v, double min, double max)
{
    return std::min(max, std::max(min, v));
}

/**
 * sigmoid
 * 
 * Algorithm to truncate values beyond the min and max value using the sigmoid
 * algorithm to smoothly clamp the values between the min and max value.
 * 
 * @param double v : The value to be truncated.
 * @param double min : The minimum value.
 * @param double max : The maximum value.
 * 
 * @returns double : The truncated value.
 */
double ConverterClampNode::sigmoid(double v, double min, double max)
{
    double ea = exp(v);
    return (ea / (ea + 1.00)) * (max - min) + min;
}

/**
 * inputConnectionDeleted @slot
 * 
 * Called when an input connection is deleted, this usually resets some data and
 * regenerates the output data.
 * 
 * @param QtNodes::Connection const& connection : The connection being deleted.
 * 
 * @signals dataUpdated
 */
void ConverterClampNode::inputConnectionDeleted(
    QtNodes::Connection const &connection)
{
    int port = (int)connection.getPortIndex(QtNodes::PortType::In);
    switch (port)
    {
    // Reset input data
    case 0:
        this->_set = false;
        this->_output = IntensityMap(1, 1, 1.00);
        emit this->dataUpdated(0);
        break;
    // Reset minimum data
    case 1:
        this->_set_min = false;
        this->_ui.spin_min->setReadOnly(false);
        this->_shared_ui.spin_min->setReadOnly(false);
        this->_generate();
        break;

    // Reset maximum data
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

/**
 * _generate
 * 
 * Generates the output data from the supplied and available data.
 * 
 * @signals dataUpdated
 */
void ConverterClampNode::_generate()
{
    Q_CHECK_PTR(this->_input);
    IntensityMap map = this->_input->intensityMap();
    this->_output = IntensityMap(map.width, map.height);
    IntensityMap min;
    IntensityMap max;

    // Set minimum and maximum values
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

    // Apply the clamping based on the mode
    switch (this->_mode)
    {
    case ConverterClampNode::CLAMP:
        for (int y = 0; y < map.height; y++)
            for (int x = 0; x < map.width; x++)
                this->_output.append(ConverterClampNode::clamp(map.at(x, y),
                                                               min.at(x, y),
                                                               max.at(x, y)));
        break;
    case ConverterClampNode::SIGMOID:
        for (int y = 0; y < map.height; y++)
            for (int x = 0; x < map.width; x++)
                this->_output.append(ConverterClampNode::sigmoid(map.at(x, y),
                                                                 min.at(x, y),
                                                                 max.at(x, y)));
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
    emit this->dataUpdated(0);
}