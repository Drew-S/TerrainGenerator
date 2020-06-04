#pragma once

#include <QJsonObject>
#include <QObject>

#include <nodes/Connection>
#include <nodes/NodeDataModel>

#include "../Datatypes/intensitymap.h"
#include "../Datatypes/pixmap.h"
#include "./BezierCurveWidget/bezierwidget.h"

#include "node.h"

/**
 * ConverterBezierCurveNode
 * 
 * Node that displays a complex slope and curve editing widget used for
 * adjusting values as a function of f(x).
 * 
 * Converts an intensity map value x into a new intensity value x' with a
 * function f(x) that is defined by a series of curves and slopes created in the
 * widget. The x value is calculated via finding the vertical line intercept on
 * the line segments of the slopes.
 */
class ConverterBezierCurveNode : public Node
{
    Q_OBJECT
public:
    // Create and setup the node
    ConverterBezierCurveNode();

    // Setup the listeners
    void created() override;

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The embedded control widget (displayed on node)
    QWidget *embeddedWidget();

    // The shared control widget (displayed in properties)
    QWidget *sharedWidget();

    // Get the number of ports (1 output, 4 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype
    QtNodes::NodeDataType
    dataType(QtNodes::PortType port_type,
             QtNodes::PortIndex port_index) const override;

    // Get the output data (the VectorMapData)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Save and load the node
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Set the input node
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data,
                   QtNodes::PortIndex port);

public slots:
    // Reset to use constant values when input removed
    void inputConnectionDeleted(QtNodes::Connection const &connection);

private:
    // Generate data
    void _generate();

    IntensityMap _output{1, 1, 1.00};

    std::shared_ptr<IntensityMapData> _input;

    bool _set = false;

    BezierEditWidget *_widget;
    BezierEditWidget *_shared_widget;
};