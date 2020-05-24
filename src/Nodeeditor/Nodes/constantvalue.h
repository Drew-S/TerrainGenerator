#pragma once

#include <QWidget>
#include <QObject>

#include <nodes/NodeDataModel>

#include "../Datatypes/pixmap.h"

#include "../Datatypes/intensitymap.h"

#include "ui_ConstantValue.h"

#include "node.h"

class InputConstantValueNode : public Node
{
    Q_OBJECT
    friend class InputConstantValueNode_Test;

public:
    InputConstantValueNode();
    ~InputConstantValueNode();

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

    // Get the port datatype (only imports intensityMapData)
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Save and load the node for project files
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Get the output data
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Set the input intensity maps or constant
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port);

private:
    // The output values
    double _value = 0.00;

    // UI
    Ui::ConstantValue _ui;
    Ui::ConstantValue _shared_ui;
    QWidget *_widget;
    QWidget *_shared_widget;
};