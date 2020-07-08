#include "bezier.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

/**
 * ConverterBezierCurveNode
 * 
 * Creates a bezier curve node and creates the control widgets.
 */
ConverterBezierCurveNode::ConverterBezierCurveNode()
{
    this->_widget = new BezierEditWidget();
    this->_shared_widget = new BezierEditWidget();
}

/**
 * created
 * 
 * Function is called when the node is created so it can connect to listeners.
 */
void ConverterBezierCurveNode::created()
{
    Q_CHECK_PTR(this->_widget);
    Q_CHECK_PTR(this->_shared_widget);
    QObject::connect(this->_widget, &BezierEditWidget::curveChanged, [this]() {
        Q_CHECK_PTR(this->_widget);
        Q_CHECK_PTR(this->_shared_widget);
        this->_shared_widget->restore(this->_widget->save());
        if (this->_set)
        {
            this->_generate();
            return;
        }
    });
    QObject::connect(this->_shared_widget,
                     &BezierEditWidget::curveChanged,
                     [this]()
    {
        Q_CHECK_PTR(this->_widget);
        Q_CHECK_PTR(this->_shared_widget);
        this->_widget->restore(this->_shared_widget->save());
        if (this->_set)
        {
            this->_generate();
            return;
        }
    });
}

/**
 * caption
 * 
 * Return a string that is displayed on the node and in the properties.
 * 
 * @returns QString The caption.
 */
QString ConverterBezierCurveNode::caption() const
{
    return QString("Slope and Curve");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString ConverterBezierCurveNode::name() const
{
    return QString("Slope and Curve transform");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram.
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *ConverterBezierCurveNode::embeddedWidget()
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
QWidget *ConverterBezierCurveNode::sharedWidget()
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
unsigned int ConverterBezierCurveNode::nPorts(QtNodes::PortType port_type) const
{
    Q_UNUSED(port_type);
    return 1;
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
ConverterBezierCurveNode::dataType(QtNodes::PortType port_type,
                                   QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    return IntensityMapData().type();
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
ConverterBezierCurveNode::outData(QtNodes::PortIndex port)
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
QJsonObject ConverterBezierCurveNode::save() const
{
    Q_CHECK_PTR(this->_widget);

    QJsonObject data;
    data["name"] = this->name();
    std::vector<QPointF> save_data = this->_widget->save();
    QJsonArray values;

    for (int i = 0; i < (int)save_data.size(); i++)
    {
        QJsonObject point;
        point["x"] = save_data[i].x();
        point["y"] = save_data[i].y();
        values << point;
    }

    data["values"] = values;
    return data;
}

/**
 * restore
 * 
 * Restores the state of the node from a provided json object.
 * 
 * @param QJsonObject const& data : The data to restore from.
 */
void ConverterBezierCurveNode::restore(QJsonObject const &data)
{
    Q_CHECK_PTR(this->_widget);
    Q_CHECK_PTR(this->_shared_widget);

    std::vector<QPointF> save_data;
    QJsonArray values = data["values"].toArray();

    for (int i = 0; i < values.count(); i++)
    {
        QJsonObject point = values.at(i).toObject();
        double x = point["x"].toDouble(0.00);
        double y = point["y"].toDouble(0.00);
        save_data.push_back(QPointF(x, y));
    }

    this->_widget->restore(save_data);
    this->_shared_widget->restore(save_data);
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
void ConverterBezierCurveNode::inputConnectionDeleted(
    QtNodes::Connection const &connection)
{
    Q_UNUSED(connection);
    this->_output = IntensityMap(1, 1, 1.00);
    this->_set = true;
    emit this->dataUpdated(0);
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
void ConverterBezierCurveNode::setInData(
    std::shared_ptr<QtNodes::NodeData> node_data,
    QtNodes::PortIndex port)
{
    Q_UNUSED(port);

    if (node_data && (this->_input = std::dynamic_pointer_cast<IntensityMapData>(node_data)))
    {
        this->_set = true;
        this->_generate();
    }
}

/**
 * _generate
 * 
 * Generates the output data from the supplied and available data.
 * 
 * @signals dataUpdated
 */
void ConverterBezierCurveNode::_generate()
{
    Q_CHECK_PTR(this->_input);
    Q_CHECK_PTR(this->_widget);
    IntensityMap map = this->_input->intensityMap();

    if (map.usingFill())
    {
        double v = this->_widget->valueAt(map.at(0, 0));
        this->_output = IntensityMap(map.width, map.height, v);
    }
    else
    {
        this->_output = IntensityMap(map.width, map.height);
        
        for (int y = 0; y < map.height; y++)
            for (int x = 0; x < map.width; x++)
                this->_output.append(this->_widget->valueAt(map.at(x, y)));
    }

    emit this->dataUpdated(0);
}
