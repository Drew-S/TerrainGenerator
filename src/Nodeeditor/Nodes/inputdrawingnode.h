#pragma once

#include "../Datatypes/intensitymap.h"

#include "ui_InputDrawingNode.h"

#include <nodes/NodeDataModel>

#include <QPixmap>
#include <QPainter>
#include <QLabel>
#include <QPointF>
#include <QWidget>
#include <QJsonObject>

class InputDrawingNode : public QtNodes::NodeDataModel
{
    Q_OBJECT
public:
    InputDrawingNode();
    ~InputDrawingNode();

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

    // Get the number of ports (1 output, 0 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only exports VectorMapData)
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Get the output data (the VectorMapData)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Needed for all nodes, even if there are no inputs
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    IntensityMap _intensity_map;
    Ui::InputDrawingNodeWidget _ui;

    bool _prev_first = false;
    QPointF _prev;

    QLabel *_widget;
    QWidget *_shared;
    QPixmap *_pixmap;
    QPainter *_painter;
};