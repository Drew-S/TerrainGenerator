#pragma once

#include <QObject>
#include <QWidget>

#include <nodes/Connection>
#include <nodes/NodeDataModel>

#include "../Datatypes/intensitymap.h"
#include "../Datatypes/pixmap.h"
#include "node.h"

#include "ui_MathNode.h"

/**
 * ConverterSmoothNode
 * 
 * Node that smooths a surface using a smoothing surface of weighted neighbor
 * contributions.
 */
class ConverterSmoothNode : public Node
{
    Q_OBJECT
    friend class ConverterSmoothNode_Test;

public:
    
    // Create the node
    ConverterSmoothNode();

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The embedded widget shown in the node
    QWidget *embeddedWidget();

    // The shared widget shown in the properties panel
    QWidget *sharedWidget();

    // Get the number of ports (1 output, 2 input)
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

public slots:
    // Reset to use constant values when input removed
    void inputConnectionDeleted(QtNodes::Connection const &connection);

private:
    // Generate the output pixmap
    void _generate();

    std::shared_ptr<IntensityMapData> _input;

    bool _set = false;

    double v = 5.00;

    IntensityMap _output;
};