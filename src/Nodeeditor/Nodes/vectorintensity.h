#pragma once

#include <QJsonObject>
#include <QObject>

#include <glm/vec4.hpp>
#include <nodes/Connection>
#include <nodes/NodeDataModel>

#include "../Datatypes/intensitymap.h"
#include "../Datatypes/pixmap.h"
#include "../Datatypes/vectormap.h"
#include "node.h"

#include "ui_VectorIntensityNode.h"

/**
 * ConverterVectorIntensityNode
 * 
 * Node to let users select a specific method from IntensityMap mode to convert
 * a vector map to an intensity map.
 */
class ConverterVectorIntensityNode : public Node
{
    Q_OBJECT
public:
    // Create the node
    ConverterVectorIntensityNode();

    // When the node is created attach listeners
    void created() override;

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The embedded widget in the node
    QWidget *embeddedWidget();

    // The shared widget in the properties panel
    QWidget *sharedWidget();

    // Get the number of ports (1 output, 2 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype
    QtNodes::NodeDataType
    dataType(QtNodes::PortType port_type,
             QtNodes::PortIndex port_index) const override;

    // Get the output data (the IntensityMapData)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Save and load the node
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Needed for all nodes, even if there are no inputs
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data,
                   QtNodes::PortIndex port_index);

public slots:
    // Reset to use constant values when input removed
    void inputConnectionDeleted(QtNodes::Connection const &connection);

    // Called when the mode changes
    void modeChanged(int index);

private:
    // Generate the output
    void _generate();
    std::shared_ptr<VectorMapData> _input;
    bool _set = false;

    // The selected channel to apply to the vector map to create the output
    IntensityMap::Channel _channel = IntensityMap::AVERAGE;

    // The output
    IntensityMap _output{1, 1, 1.00};

    // UI
    Ui::VectorIntensityNode _ui;
    Ui::VectorIntensityNode _shared_ui;
    QWidget *_widget;
    QWidget *_shared_widget;
};