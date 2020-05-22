#include "vectorintensity.h"

#include "Globals/settings.h"

#include <QComboBox>
#include <QDebug>

ConverterVectorIntensityNode::ConverterVectorIntensityNode()
{
    qDebug("Creating Vector to Intensity Converter Node, attaching listeners and UI widget");
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();

    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);
}

ConverterVectorIntensityNode::~ConverterVectorIntensityNode() {}

void ConverterVectorIntensityNode::created()
{
    QObject::connect(this->_ui.mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConverterVectorIntensityNode::modeChanged);
    QObject::connect(this->_shared_ui.mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConverterVectorIntensityNode::modeChanged);

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

// Title shown at the top of the node
QString
ConverterVectorIntensityNode::caption() const
{
    return QString("Vector to Intensity Map");
}

// Title shown in the selection list
QString ConverterVectorIntensityNode::name() const
{
    return QString("Vector to Intensity Converter");
}

// The image label that is embedded in the node
QWidget *ConverterVectorIntensityNode::embeddedWidget()
{
    Q_CHECK_PTR(this->_widget);
    return this->_widget;
}
QWidget *ConverterVectorIntensityNode::sharedWidget()
{
    Q_CHECK_PTR(this->_shared_widget);
    return this->_shared_widget;
}

// Get the number of ports (1 output, 1 input)
unsigned int ConverterVectorIntensityNode::nPorts(QtNodes::PortType port_type) const
{
    Q_UNUSED(port_type);
    return 1;
}

// Get the port datatype
QtNodes::NodeDataType ConverterVectorIntensityNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_index);
    return port_type == QtNodes::PortType::In ? VectorMapData().type() : IntensityMapData().type();
}

// Get the output data (the IntensityMapData)
std::shared_ptr<QtNodes::NodeData> ConverterVectorIntensityNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return std::make_shared<IntensityMapData>(this->_output);
}

// Save and load the node
QJsonObject ConverterVectorIntensityNode::save() const
{
    qDebug("Saving vector to intensity converter node");
    QJsonObject data;
    data["name"] = this->name();
    data["channel"] = (int)this->_channel;
    return data;
}
void ConverterVectorIntensityNode::restore(QJsonObject const &data)
{
    qDebug("Restoring vector to intensity converter node");
    this->_channel = (IntensityMap::Channel)data["channel"].toInt(0);
}

// Needed for all nodes, even if there are no inputs
void ConverterVectorIntensityNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port_index)
{
    Q_UNUSED(port_index);
    if (node_data)
    {
        this->_input = std::dynamic_pointer_cast<VectorMapData>(node_data);
        this->_set = true;
        this->_generate();
    }
}

// Generate resulting output
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

// When the connection is deleted use default value
void ConverterVectorIntensityNode::inputConnectionDeleted(QtNodes::Connection const &connection)
{
    Q_UNUSED(connection);
    this->_set = false;
    this->_generate();
}

// When the mode changes
void ConverterVectorIntensityNode::modeChanged(int index)
{
    this->_channel = (IntensityMap::Channel)index;
    this->_ui.mode->setCurrentIndex(index);
    this->_shared_ui.mode->setCurrentIndex(index);
    this->_generate();
}