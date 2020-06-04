#include "vectorintensity.h"

#include <QComboBox>
#include <QDebug>

#include "Globals/settings.h"

/**
 * ConverterVectorIntensityNode
 * 
 * Creates the node and the UI.
 */
ConverterVectorIntensityNode::ConverterVectorIntensityNode()
{
    qDebug("Creating Vector to Intensity Converter Node and UI widgets");
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
void ConverterVectorIntensityNode::created()
{
    QObject::connect(this->_ui.mode,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     this,
                     &ConverterVectorIntensityNode::modeChanged);

    QObject::connect(this->_shared_ui.mode,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     this,
                     &ConverterVectorIntensityNode::modeChanged);

    Q_CHECK_PTR(SETTINGS);
    QObject::connect(SETTINGS, &Settings::previewResolutionChanged, [this]() {
        Q_CHECK_PTR(SETTINGS);
        if (SETTINGS->renderMode())
            return;
        int size = SETTINGS->previewResolution();
        this->_output.width = size;
        this->_output.height = size;
        emit this->dataUpdated(0);
    });
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
    QObject::connect(SETTINGS, &Settings::renderModeChanged, [this]() {
        Q_CHECK_PTR(SETTINGS);
        if (!_set)
        {
            int size;
            if (SETTINGS->renderMode())
                size = SETTINGS->renderResolution();
            else
                size = SETTINGS->previewResolution();
            this->_output.width = size;
            this->_output.height = size;
            emit this->dataUpdated(0);
        }
    });
}

/**
 * caption
 * 
 * Return a string that is displayed on the node and in the properties.
 * 
 * @returns QString : The caption.
 */
QString ConverterVectorIntensityNode::caption() const
{
    return QString("Vector to Intensity Map");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString ConverterVectorIntensityNode::name() const
{
    return QString("Vector to Intensity Converter");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram.
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *ConverterVectorIntensityNode::embeddedWidget()
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
QWidget *ConverterVectorIntensityNode::sharedWidget()
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
ConverterVectorIntensityNode::nPorts(QtNodes::PortType port_type) const
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
ConverterVectorIntensityNode::dataType(QtNodes::PortType port_type,
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
ConverterVectorIntensityNode::outData(QtNodes::PortIndex port)
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
QJsonObject ConverterVectorIntensityNode::save() const
{
    qDebug("Saving vector to intensity converter node");
    QJsonObject data;
    data["name"] = this->name();
    data["channel"] = (int)this->_channel;
    return data;
}

/**
 * restore
 * 
 * Restores the state of the node from a provided json object.
 * 
 * @param QJsonObject const& data : The data to restore from.
 */
void ConverterVectorIntensityNode::restore(QJsonObject const &data)
{
    qDebug("Restoring vector to intensity converter node");
    this->_channel = (IntensityMap::Channel)data["channel"].toInt(0);
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
void ConverterVectorIntensityNode::setInData(
    std::shared_ptr<QtNodes::NodeData> node_data,
    QtNodes::PortIndex port_index)
{
    Q_UNUSED(port_index);
    if (node_data)
    {
        this->_input = std::dynamic_pointer_cast<VectorMapData>(node_data);
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
void ConverterVectorIntensityNode::_generate()
{
    qDebug("Generating output");
    if (this->_set)
    {
        VectorMap map = this->_input->vectorMap();
        this->_output = map.toIntensityMap(this->_channel);
    }
    emit this->dataUpdated(0);
}

/**
 * inputConnectionDeleted @slot
 * 
 * Called when an input connection is deleted, this usually resets some data and
 * regenerates the output data.
 * 
 * @param QtNodes::Connection const& connection : The connection being deleted.
 */
void ConverterVectorIntensityNode::inputConnectionDeleted(
    QtNodes::Connection const &connection)
{
    Q_UNUSED(connection);
    this->_set = false;
    this->_generate();
}

/**
 * modeChanged @slot
 * 
 * Called to change the mode used to convert the vector map. Regenerates the
 * output.
 * 
 * @param int index : The new mode.
 */
void ConverterVectorIntensityNode::modeChanged(int index)
{
    this->_channel = (IntensityMap::Channel)index;
    this->_ui.mode->setCurrentIndex(index);
    this->_shared_ui.mode->setCurrentIndex(index);
    this->_generate();
}