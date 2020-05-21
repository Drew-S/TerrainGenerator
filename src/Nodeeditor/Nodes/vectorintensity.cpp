#include "vectorintensity.h"

#include <QComboBox>

ConverterVectorIntensityNode::ConverterVectorIntensityNode()
{
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();

    this->_ui.setupUi(this->_widget);
    this->_shared_ui.setupUi(this->_shared_widget);

    QObject::connect(this->_ui.mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConverterVectorIntensityNode::modeChanged);
    QObject::connect(this->_shared_ui.mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConverterVectorIntensityNode::modeChanged);
}

ConverterVectorIntensityNode::~ConverterVectorIntensityNode() {}

// Title shown at the top of the node
QString ConverterVectorIntensityNode::caption() const
{
    return QString("Vector to Intensity Map Converter");
}

// Title shown in the selection list
QString ConverterVectorIntensityNode::name() const
{
    return QString("VectorIntensityConverter");
}
void ConverterVectorIntensityNode::name(QString name)
{
    (void)name;
}

// The image label that is embedded in the node
QWidget *ConverterVectorIntensityNode::embeddedWidget()
{
    return this->_widget;
}
QWidget *ConverterVectorIntensityNode::sharedWidget()
{
    return this->_shared_widget;
}

// Needed for NodeDataModel, not sure where it is used
QString ConverterVectorIntensityNode::modelName()
{
    return QString("Vector Intensity Map Converter");
}

// Get the number of ports (1 output, 1 input)
unsigned int ConverterVectorIntensityNode::nPorts(QtNodes::PortType port_type) const
{
    (void)port_type;
    return 1;
}

// Get the port datatype
QtNodes::NodeDataType ConverterVectorIntensityNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    (void)port_index;
    return port_type == QtNodes::PortType::In ? VectorMapData().type() : IntensityMapData().type();
}

// Get the output data (the IntensityMapData)
std::shared_ptr<QtNodes::NodeData> ConverterVectorIntensityNode::outData(QtNodes::PortIndex port)
{
    return std::make_shared<IntensityMapData>(this->_output);
}

// Save and load the node
QJsonObject ConverterVectorIntensityNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    return data;
}
void ConverterVectorIntensityNode::restore(QJsonObject const &data)
{
    (void)data;
}

// Needed for all nodes, even if there are no inputs
void ConverterVectorIntensityNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port_index)
{
    (void)port_index;
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
    (void)connection;
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