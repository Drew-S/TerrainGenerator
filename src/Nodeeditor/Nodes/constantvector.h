#pragma once

#include <QWidget>
#include <QObject>

#include <nodes/NodeDataModel>

#include "../Datatypes/pixmap.h"

#include "../Datatypes/vectormap.h"

#include "ui_ConstantVector.h"

#include "node.h"

class InputConstantVectorNode : public Node
{
    Q_OBJECT
    friend class InputConstantVectorNode_Test;

public:
    InputConstantVectorNode();
    ~InputConstantVectorNode();

    // When the node is created attach listeners
    void created() override;

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The image label that is embedded in the node
    QWidget *embeddedWidget();
    QWidget *sharedWidget();

    // Get the number of ports (1 output, 0 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only imports VectorMapData)
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Save and load the node for project files
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Get the output data
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Set the input intensity maps or constant
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port);

private:
    // The output value
    double _x = 0.00;
    double _y = 0.00;
    double _z = 0.00;
    double _w = 0.00;

    // UI
    Ui::ConstantVector _ui;
    Ui::ConstantVector _shared_ui;
    QWidget *_widget;
    QWidget *_shared_widget;
};