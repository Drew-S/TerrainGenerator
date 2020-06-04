#include "vectordot.h"

#include <QDebug>
#include <QDoubleSpinBox>

#include "Globals/settings.h"

/**
 * ConverterVectorDotNode
 * 
 * Creates the node and the UI.
 */
ConverterVectorDotNode::ConverterVectorDotNode()
{
    qDebug("Creating Vector Dot Product Node, attaching listeners and UI widget");
    // Setup ui
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
void ConverterVectorDotNode::created()
{
    // Attach vector 1 in embedded ui listeners
    QObject::connect(this->_ui.spin_x_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::x0Changed);

    QObject::connect(this->_ui.spin_y_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::y0Changed);

    QObject::connect(this->_ui.spin_z_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::z0Changed);

    QObject::connect(this->_ui.spin_w_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::w0Changed);

    // Attach vector 2 in embedded ui listeners
    QObject::connect(this->_ui.spin_x_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::x1Changed);

    QObject::connect(this->_ui.spin_y_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::y1Changed);

    QObject::connect(this->_ui.spin_z_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::z1Changed);

    QObject::connect(this->_ui.spin_w_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::w1Changed);

    // Attach vector 1 in shared ui listeners
    QObject::connect(this->_shared_ui.spin_x_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::x0Changed);

    QObject::connect(this->_shared_ui.spin_y_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::y0Changed);

    QObject::connect(this->_shared_ui.spin_z_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::z0Changed);

    QObject::connect(this->_shared_ui.spin_w_0,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::w0Changed);

    // Attach vector 2 in shared ui listeners
    QObject::connect(this->_shared_ui.spin_x_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::x1Changed);

    QObject::connect(this->_shared_ui.spin_y_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::y1Changed);

    QObject::connect(this->_shared_ui.spin_z_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::z1Changed);

    QObject::connect(this->_shared_ui.spin_w_1,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &ConverterVectorDotNode::w1Changed);

    QObject::connect(SETTINGS, &Settings::renderModeChanged, [this]() {
        if (!this->_in_set_0 && !this->_in_set_1)
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
QString ConverterVectorDotNode::caption() const
{
    return QString("Vector Dot Product");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString ConverterVectorDotNode::name() const
{
    return QString("Vector Dot Product");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram.
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *ConverterVectorDotNode::embeddedWidget()
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
QWidget *ConverterVectorDotNode::sharedWidget()
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
unsigned int ConverterVectorDotNode::nPorts(QtNodes::PortType port_type) const
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
ConverterVectorDotNode::dataType(QtNodes::PortType port_type,
                                 QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_index);
    return port_type == QtNodes::PortType::In ? VectorMapData().type()
                                              : IntensityMapData().type();
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
ConverterVectorDotNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return std::make_shared<IntensityMapData>(this->_output);
}

/**
 * save
 * 
 * Saves the state of the node into a QJsonObject for the system to save to
 * file.
 * 
 * @returns QJsonObject : The saved state of the node.
 */
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

/**
 * restore
 * 
 * Restores the state of the node from a provided json object.
 * 
 * @param QJsonObject const& data : The data to restore from.
 */
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

/**
 * setInData
 * 
 * Sets the input data on a port.
 * 
 * @param std::shared_ptr<QtNodes::NodeData> node_data : The shared pointer data
 *                                                       being inputted.
 * @param QtNodes::PortIndex port : The port the data is being set on.
 */
void ConverterVectorDotNode::setInData(
    std::shared_ptr<QtNodes::NodeData> node_data,
    QtNodes::PortIndex port_index)
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
        default:
            Q_UNREACHABLE();
            break;
        }
        this->_generate();
    }
}

/**
 * inputConnectionDeleted @slot
 * 
 * Called when an input connection is deleted, this usually resets some data and
 * regenerates the output data.
 * 
 * @param QtNodes::Connection const& connection : The connection being deleted.
 */
void ConverterVectorDotNode::inputConnectionDeleted(
    QtNodes::Connection const &connection)
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
    default:
        Q_UNREACHABLE();
        break;
    }
    this->_generate();
}

/**
 * x0Changed
 * 
 * Called to updated the first vector when the x value changed.
 * 
 * @param double value : The new value.
 */
void ConverterVectorDotNode::x0Changed(double value)
{
    this->_in_val_0.x = value;
    this->_ui.spin_x_0->setValue(this->_in_val_0.x);
    this->_shared_ui.spin_x_0->setValue(this->_in_val_0.x);
    this->_generate();
}

/**
 * y0Changed
 * 
 * Called to updated the first vector when the y value changed.
 * 
 * @param double value : The new value.
 */
void ConverterVectorDotNode::y0Changed(double value)
{
    this->_in_val_0.y = value;
    this->_ui.spin_y_0->setValue(this->_in_val_0.y);
    this->_shared_ui.spin_y_0->setValue(this->_in_val_0.y);
    this->_generate();
}

/**
 * z0Changed
 * 
 * Called to updated the first vector when the z value changed.
 * 
 * @param double value : The new value.
 */
void ConverterVectorDotNode::z0Changed(double value)
{
    this->_in_val_0.z = value;
    this->_ui.spin_z_0->setValue(this->_in_val_0.z);
    this->_shared_ui.spin_z_0->setValue(this->_in_val_0.z);
    this->_generate();
}

/**
 * w0Changed
 * 
 * Called to updated the first vector when the w value changed.
 * 
 * @param double value : The new value.
 */
void ConverterVectorDotNode::w0Changed(double value)
{
    this->_in_val_0.w = value;
    this->_ui.spin_w_0->setValue(this->_in_val_0.w);
    this->_shared_ui.spin_w_0->setValue(this->_in_val_0.w);
    this->_generate();
}

/**
 * x1Changed
 * 
 * Called to updated the second vector when the x value changed.
 * 
 * @param double value : The new value.
 */
void ConverterVectorDotNode::x1Changed(double value)
{
    this->_in_val_1.x = value;
    this->_ui.spin_x_1->setValue(this->_in_val_1.x);
    this->_shared_ui.spin_x_1->setValue(this->_in_val_1.x);
    this->_generate();
}

/**
 * y1Changed
 * 
 * Called to updated the second vector when the y value changed.
 * 
 * @param double value : The new value.
 */
void ConverterVectorDotNode::y1Changed(double value)
{
    this->_in_val_1.y = value;
    this->_ui.spin_y_1->setValue(this->_in_val_1.y);
    this->_shared_ui.spin_y_1->setValue(this->_in_val_1.y);
    this->_generate();
}

/**
 * z1Changed
 * 
 * Called to updated the second vector when the z value changed.
 * 
 * @param double value : The new value.
 */
void ConverterVectorDotNode::z1Changed(double value)
{
    this->_in_val_1.z = value;
    this->_ui.spin_z_1->setValue(this->_in_val_1.z);
    this->_shared_ui.spin_z_1->setValue(this->_in_val_1.z);
    this->_generate();
}

/**
 * w1Changed
 * 
 * Called to updated the second vector when the w value changed.
 * 
 * @param double value : The new value.
 */
void ConverterVectorDotNode::w1Changed(double value)
{
    this->_in_val_1.w = value;
    this->_ui.spin_w_1->setValue(this->_in_val_1.w);
    this->_shared_ui.spin_w_1->setValue(this->_in_val_1.w);
    this->_generate();
}

/**
 * _generate
 * 
 * Generates the output data from the supplied and available data.
 * 
 * @signals dataUpdated
 */
void ConverterVectorDotNode::_generate()
{
    qDebug("Applying tranformation, generating output");
    VectorMap map_0;
    VectorMap map_1;
    if (this->_in_set_0)
    {
        Q_CHECK_PTR(this->_in_0);
        map_0 = this->_in_0->vectorMap();
    }
    else
    {
        map_0 = VectorMap(1, 1, this->_in_val_0);
    }

    if (this->_in_set_1)
    {
        Q_CHECK_PTR(this->_in_1);
        map_1 = this->_in_1->vectorMap();
    }
    else
    {
        map_1 = VectorMap(1, 1, this->_in_val_1);
    }

    if (map_0.usingFill() && map_1.usingFill())
    {
        this->_output = IntensityMap(
                    map_0.width,
                    map_0.height,
                    ConverterVectorDotNode::dot(
                        map_0.at(0, 0),
                        map_1.at(0, 0)));
    }
    else
    {
        this->_output = IntensityMap(map_0.width, map_0.height);

        for (int y = 0; y < map_0.height; y++)
            for (int x = 0; x < map_0.width; x++)
                this->_output.append(ConverterVectorDotNode::dot(
                    map_0.at(x, y),
                    map_1.at(x, y)));
    }

    emit this->dataUpdated(0);
}

/**
 * dot
 * 
 * Algorithm for applying the vector dot product to the two vectors.
 * 
 * @param glm::dvec4 a : The first vector.
 * @param glm::dvec4 b : The second vector.
 * 
 * @returns double : The resulting value.
 */
double ConverterVectorDotNode::dot(glm::dvec4 a, glm::dvec4 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}