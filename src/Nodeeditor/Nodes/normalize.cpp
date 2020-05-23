#include "normalize.h"

#include <math.h>

ConverterNormalizeNode::ConverterNormalizeNode() {}

ConverterNormalizeNode::~ConverterNormalizeNode() {}

QString ConverterNormalizeNode::caption() const
{
    return QString("Normalize");
}

QString ConverterNormalizeNode::name() const
{
    return QString("Normalize");
}

QWidget *ConverterNormalizeNode::embeddedWidget()
{
    return nullptr;
}

unsigned int ConverterNormalizeNode::nPorts(QtNodes::PortType port_type) const
{
    Q_UNUSED(port_type);
    return 1;
}

QtNodes::NodeDataType ConverterNormalizeNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    return VectorMapData().type();
}

QJsonObject ConverterNormalizeNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    return data;
}

void ConverterNormalizeNode::restore(QJsonObject const &data)
{
    Q_UNUSED(data);
}

std::shared_ptr<QtNodes::NodeData> ConverterNormalizeNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return std::make_shared<VectorMapData>(this->_output);
}

void ConverterNormalizeNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    if (node_data)
    {
        this->_input = std::dynamic_pointer_cast<VectorMapData>(node_data);
        this->_set = true;
        this->_generate();
    }
}

void ConverterNormalizeNode::inputConnectionDeleted(QtNodes::Connection const &connection)
{
    Q_UNUSED(connection);
    this->_set = false;
    this->_output = VectorMap(1.00, 1.00, glm::dvec4(0.00, 0.00, 0.00, 1.00));
    emit this->dataUpdated(0);
}

glm::dvec4 ConverterNormalizeNode::normalize(glm::dvec4 a, glm::dvec4 b)
{
    Q_UNUSED(b);
    double sum = 1.00 / sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z) + (a.w * a.w));
    return a * sum;
}

void ConverterNormalizeNode::_generate()
{
    Q_CHECK_PTR(this->_input);
    VectorMap map = this->_input->vectorMap();
    map.transform(&ConverterNormalizeNode::normalize, glm::dvec4());
    this->_output = map;
    emit this->dataUpdated(0);
}