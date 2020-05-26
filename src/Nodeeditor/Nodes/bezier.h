#pragma once

#include "../Datatypes/intensitymap.h"
#include "../Datatypes/pixmap.h"

#include "./BezierCurveWidget/bezierwidget.h"

#include <nodes/NodeDataModel>
#include <nodes/Connection>

#include <QObject>
#include <QJsonObject>

#include "node.h"

class ConverterBezierCurveNode : public Node
{
    Q_OBJECT
public:
    ConverterBezierCurveNode();
    ~ConverterBezierCurveNode();

    void created() override;

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The embedded control widget
    QWidget *embeddedWidget();

    // Get the number of ports (1 output, 4 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Get the output data (the VectorMapData)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Save and load the node
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Set the input node
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port);

public slots:
    // Reset to use constant values when input removed
    void inputConnectionDeleted(QtNodes::Connection const &connection);

private:
    void _generate();

    IntensityMap _output{1, 1, 1.00};

    std::shared_ptr<IntensityMapData> _input;

    bool _set = false;

    BezierCurveEditWidget *_widget;
};