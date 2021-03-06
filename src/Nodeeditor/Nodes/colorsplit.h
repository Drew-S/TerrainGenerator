#pragma once

#include <QJsonObject>
#include <QObject>

#include <nodes/Connection>
#include <nodes/NodeDataModel>

#include "../Datatypes/intensitymap.h"
#include "../Datatypes/pixmap.h"
#include "node.h"

/**
 * ConverterColorSplitNode
 * 
 * This node takes a vector map input and splits out each channel into its own
 * intensity map output.
 */
class ConverterColorSplitNode : public Node
{
    Q_OBJECT
    friend class ConverterColorSplitNode_Test;

public:
    // Creates the new node
    ConverterColorSplitNode();

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The image label that is embedded in the node
    QWidget *embeddedWidget();

    // Get the number of ports (4 output, 1 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Get the output data (the IntensityMapData)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Save and load the node
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Needed for all nodes, even if there are no inputs
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port);

public slots:
    // Reset to use constant values when input removed
    void inputConnectionDeleted(QtNodes::Connection const &connection);

private:
    // Splits the vector map to individual channels
    void _generate();

    // Individual channels for the output
    IntensityMap _red{1, 1, 1.00};
    IntensityMap _green{1, 1, 1.00};
    IntensityMap _blue{1, 1, 1.00};
    IntensityMap _alpha{1, 1, 1.00};

    std::shared_ptr<VectorMapData> _input;
    bool _set = false;
};
