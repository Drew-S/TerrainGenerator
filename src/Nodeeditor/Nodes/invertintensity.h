#pragma once

#include "../Datatypes/intensitymap.h"
#include "../Datatypes/pixmap.h"
#include "node.h"

/**
 * ConverterInvertIntensityNode
 * 
 * Inverts an intensity map input by applying the function f(i) = 1 - i for
 * each pixel (i) in the map.
 */
class ConverterInvertIntensityNode : public Node
{
    Q_OBJECT
    friend class ConverterInvertIntensityNode_Test;

public:
    // Create the node
    ConverterInvertIntensityNode();

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The image label that is embedded in the node
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

    // Set the input intensity map
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data,
                   QtNodes::PortIndex port);

public slots:
    void inputConnectionDeleted(QtNodes::Connection const &connection);

private:
    // Generates the output
    void _generate();

    // Whether the input is set
    bool _set = false;

    // The output value
    IntensityMap _output{1, 1, 1.00};

    // The input value
    std::shared_ptr<IntensityMapData> _input;
};