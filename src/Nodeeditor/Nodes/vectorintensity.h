#pragma once

#include "../Datatypes/intensitymap.h"
#include "../Datatypes/vectormap.h"
#include "../Datatypes/pixmap.h"

#include <nodes/NodeDataModel>
#include <nodes/Connection>

#include <QObject>
#include <QJsonObject>

#include <glm/vec4.hpp>

#include "ui_VectorIntensityNode.h"

class ConverterVectorIntensityNode : public QtNodes::NodeDataModel
{
    Q_OBJECT
public:
    ConverterVectorIntensityNode();
    ~ConverterVectorIntensityNode();

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;
    void name(QString name);

    // The image label that is embedded in the node
    QWidget *embeddedWidget();
    QWidget *sharedWidget();

    // Needed for NodeDataModel, not sure where it is used
    QString modelName();

    // Get the number of ports (1 output, 2 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Get the output data (the IntensityMapData)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Save and load the node
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Needed for all nodes, even if there are no inputs
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port_index);

public slots:
    // Reset to use constant values when input removed
    void inputConnectionDeleted(QtNodes::Connection const &connection);

    void modeChanged(int index);

private:
    void _generate();
    std::shared_ptr<VectorMapData> _input;
    bool _set = false;

    IntensityMap::Channel _channel = IntensityMap::AVERAGE;

    // TODO: rename other files "_pixmap" to "_output"
    IntensityMap _output{1, 1, 1.00};

    // UI
    Ui::VectorIntensityNode _ui;
    Ui::VectorIntensityNode _shared_ui;
    QWidget *_widget;
    QWidget *_shared_widget;
};