#pragma once

#include <glm/vec4.hpp>

#include <nodes/Connection>
#include <nodes/NodeDataModel>

#include "../Datatypes/pixmap.h"
#include "../Datatypes/vectormap.h"
#include "node.h"

/**
 * ConverterNormalizeNode
 * 
 * Node that is used to normalize a vector map, each pixel is normalized.
 */
class ConverterNormalizeNode : public Node
{
    Q_OBJECT
    friend class ConverterNormalizeNode_Test;

public:
    // Create the node
    ConverterNormalizeNode();

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The embedded widget in the node
    QWidget *embeddedWidget();

    // Get the number of ports (1 output, 1 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only imports VectorMapData)
    QtNodes::NodeDataType
    dataType(QtNodes::PortType port_type,
             QtNodes::PortIndex port_index) const override;

    // Save and load the node for project files
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Get the output data
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Set the input intensity maps or constant
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data,
                   QtNodes::PortIndex port);

    // Normalize each value (b is ignored, uses vector map transform function)
    static glm::dvec4 normalize(glm::dvec4 a, glm::dvec4 b);

public slots:
    // Reset to use constant values when input removed
    void inputConnectionDeleted(QtNodes::Connection const &connection);

private:
    // Generate the output
    void _generate();

    // Whether input is set
    bool _set = false;

    // The default output value
    VectorMap _output{1, 1, glm::dvec4{0.00, 0.00, 0.00, 1.00}};

    // The attached input
    std::shared_ptr<VectorMapData> _input;
};