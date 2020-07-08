#include "colorcombine.h"

#include "Globals/settings.h"

#include <QColor>
#include <QDoubleSpinBox>
#include <QDebug>

/**
 * ConverterColorCombineNode
 * 
 * Creates a converter colour combine node and setups the UI.
 * 
 * TODO: Long term, fork nodeeditor to include better stylings
 */
ConverterColorCombineNode::ConverterColorCombineNode()
{
    qDebug("Created new Color combine node, attaching listeners and UI widget");
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();

    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);
}

/**
 * created
 * 
 * Function is called when the node is created so it can connect to listeners.
 * 
 * @signals dataUpdated
 */
void ConverterColorCombineNode::created()
{
    // Listener for changing red value
    QObject::connect(this->_ui.spin_red,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterColorCombineNode::redChanged);

    // Listener for changing green value
    QObject::connect(this->_ui.spin_green,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterColorCombineNode::greenChanged);

    // Listener for changing blue value
    QObject::connect(this->_ui.spin_blue,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterColorCombineNode::blueChanged);

    // Listener for changing alpha value
    QObject::connect(this->_ui.spin_alpha,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterColorCombineNode::alphaChanged);


    // Listener for changing red value
    QObject::connect(this->_shared_ui.spin_red,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterColorCombineNode::redChanged);

    // Listener for changing green value
    QObject::connect(this->_shared_ui.spin_green,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterColorCombineNode::greenChanged);

    // Listener for changing blue value
    QObject::connect(this->_shared_ui.spin_blue,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterColorCombineNode::blueChanged);

    // Listener for changing alpha value
    QObject::connect(this->_shared_ui.spin_alpha,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterColorCombineNode::alphaChanged);

    Q_CHECK_PTR(SETTINGS);

    // When preview resolution changes update data
    QObject::connect(SETTINGS, &Settings::previewResolutionChanged, [this]() {
        Q_CHECK_PTR(SETTINGS);
        if (SETTINGS->renderMode())
            return;
        int size = SETTINGS->previewResolution();
        this->_output.width = size;
        this->_output.height = size;
        emit this->dataUpdated(0);
    });

    // When render resolution changes update data
    QObject::connect(SETTINGS, &Settings::renderResolutionChanged, [this]() {
        qDebug("Render Resolution Changed");
        Q_CHECK_PTR(SETTINGS);
        if (!SETTINGS->renderMode())
            return;
        int size = SETTINGS->renderResolution();
        this->_output.width = size;
        this->_output.height = size;
        emit this->dataUpdated(0);
    });

    // When render mode changes update data
    QObject::connect(SETTINGS, &Settings::renderModeChanged, [this]() {
        if (!this->_red_set
            && !this->_green_set
            && !this->_blue_set
            && !this->_alpha_set)
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
QString ConverterColorCombineNode::caption() const
{
    return QString("Combine Color Channels");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString ConverterColorCombineNode::name() const
{
    return QString("Colour Combine");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram.
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *ConverterColorCombineNode::embeddedWidget()
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
QWidget *ConverterColorCombineNode::sharedWidget()
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
unsigned int
ConverterColorCombineNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 4 : 1;
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
ConverterColorCombineNode::dataType(QtNodes::PortType port_type,
                                    QtNodes::PortIndex port_index) const
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
    Q_UNREACHABLE();
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
ConverterColorCombineNode::outData(QtNodes::PortIndex port)
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
void ConverterColorCombineNode::setInData(
    std::shared_ptr<QtNodes::NodeData> node_data,
    QtNodes::PortIndex port_index)
{
    if (node_data)
    {
        switch ((int)port_index)
        {
        case 0:
            if((this->_red =
                std::dynamic_pointer_cast<IntensityMapData>(node_data)))
            {
                this->_red_set = true;
                this->_ui.spin_red->setReadOnly(true);
                this->_shared_ui.spin_red->setReadOnly(true);
            }
            break;
        case 1:
            if((this->_green =
                std::dynamic_pointer_cast<IntensityMapData>(node_data)))
            {
                this->_green_set = true;
                this->_ui.spin_green->setReadOnly(true);
                this->_shared_ui.spin_green->setReadOnly(true);
            }
            break;
        case 2:
            if((this->_blue =
                std::dynamic_pointer_cast<IntensityMapData>(node_data)))
            {
                this->_blue_set = true;
                this->_ui.spin_blue->setReadOnly(true);
                this->_shared_ui.spin_blue->setReadOnly(true);
            }
            break;
        case 3:
            if((this->_alpha =
                std::dynamic_pointer_cast<IntensityMapData>(node_data)))
            {
                this->_alpha_set = true;
                this->_ui.spin_alpha->setReadOnly(true);
                this->_shared_ui.spin_alpha->setReadOnly(true);
            }
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
        this->_generate();
    }
}

/**
 * save
 * 
 * Saves the state of the node into a QJsonObject for the system to save to
 * file.
 * 
 * @returns QJsonObject : The saved state of the node.
 */
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

/**
 * restore
 * 
 * Restores the state of the node from a provided json object.
 * 
 * @param QJsonObject const& data : The data to restore from.
 */
void ConverterColorCombineNode::restore(QJsonObject const &data)
{
    qDebug("Restoring combine node");
    this->_red_val = data["red"].toDouble(1.00);
    this->_green_val = data["green"].toDouble(1.00);
    this->_blue_val = data["blue"].toDouble(1.00);
    this->_alpha_val = data["alpha"].toDouble(1.00);
}

/**
 * inputConnectionDeleted @slot
 * 
 * Called when an input connection is deleted, this usually resets some data and
 * regenerates the output data.
 * 
 * @param QtNodes::Connection const& connection : The connection being deleted.
 */
void ConverterColorCombineNode::inputConnectionDeleted(
    QtNodes::Connection const &connection)
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

/**
 * redChanged @slot
 * 
 * When the red spinbox changes we update the data.
 * 
 * @param double value : The new value.
 */
void ConverterColorCombineNode::redChanged(double value)
{
    this->_red_val = value;
    this->_ui.spin_red->setValue(this->_red_val);
    this->_shared_ui.spin_red->setValue(this->_red_val);
    this->_generate();
}

/**
 * greenChanged @slot
 * 
 * When the green spinbox changes we update the data.
 * 
 * @param double value : The new value.
 */
void ConverterColorCombineNode::greenChanged(double value)
{
    this->_green_val = value;
    this->_ui.spin_green->setValue(this->_green_val);
    this->_shared_ui.spin_green->setValue(this->_green_val);
    this->_generate();
}

/**
 * blueChanged @slot
 * 
 * When the blue spinbox changes we update the data.
 * 
 * @param double value : The new value.
 */
void ConverterColorCombineNode::blueChanged(double value)
{
    this->_blue_val = value;
    this->_ui.spin_blue->setValue(this->_blue_val);
    this->_shared_ui.spin_blue->setValue(this->_blue_val);
    this->_generate();
}

/**
 * alphaChanged @slot
 * 
 * When the alpha spinbox changes we update the data.
 * 
 * @param double value : The new value.
 */
void ConverterColorCombineNode::alphaChanged(double value)
{
    this->_alpha_val = value;
    this->_ui.spin_alpha->setValue(this->_alpha_val);
    this->_shared_ui.spin_alpha->setValue(this->_alpha_val);
    this->_generate();
}

/**
 * _generate
 * 
 * Generates the output data from the supplied and available data.
 * 
 * @signals dataUpdated
 */
void ConverterColorCombineNode::_generate()
{
    qDebug("Combinging values into output");
    if (!this->_red_set
        && !this->_green_set
        && !this->_blue_set
        && !this->_alpha_set)
    {
        this->_output = VectorMap(1, 1, glm::dvec4(this->_red_val,
                                                   this->_green_val,
                                                   this->_blue_val,
                                                   this->_alpha_val));
    }
    else
    {
        int size;
        if (SETTINGS->renderMode())
            size = SETTINGS->renderResolution();
        else
            size = SETTINGS->previewResolution();
        IntensityMap red(size, size, this->_red_val);
        IntensityMap green(size, size, this->_green_val);
        IntensityMap blue(size, size, this->_blue_val);
        IntensityMap alpha(size, size, this->_alpha_val);

        if (this->_red_set)
        {
            Q_CHECK_PTR(this->_red);
            red = this->_red->intensityMap();
        }

        if (this->_green_set)
        {
            Q_CHECK_PTR(this->_green);
            green = this->_green->intensityMap();
        }

        if (this->_blue_set)
        {
            Q_CHECK_PTR(this->_blue);
            blue = this->_blue->intensityMap();
        }

        if (this->_alpha_set)
        {
            Q_CHECK_PTR(this->_alpha);
            alpha = this->_alpha->intensityMap();
        }

        this->_output = VectorMap(size, size);
        for (int y = 0; y < size; y++)
            for (int x = 0; x < size; x++)
                this->_output.append(glm::dvec4(
                    red.at(x, y),
                    green.at(x, y),
                    blue.at(x, y),
                    alpha.at(x, y)));
    }
    emit this->dataUpdated(0);
}