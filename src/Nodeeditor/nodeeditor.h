#pragma once

#include <QLayout>
#include <QObject>
#include <QJsonObject>
#include <QWidget>

#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/Node>

#include "./Nodes/output.h"
#include "./Nodes/inputtexture.h"
#include "./Nodes/inputsimplexnoise.h"
#include "./Nodes/mathnode.h"

// Wrapper to manage the FlowView and FlowScene and listen for events
class Nodeeditor : public QObject
{
    Q_OBJECT

public:
    Nodeeditor(QLayout *target, QWidget *properties);
    ~Nodeeditor();

    // Returns the height/normal map of the active selected output node
    QImage getHeightMap();
    QImage getNormalMap();

    // Save/load the editor nodes, layout, and connections
    QJsonObject save();
    void load(QJsonObject data);

public slots:
    // Called when a node is created
    void nodeCreated(QtNodes::Node &node);
    // Called when a node is double clicked (selected as primary)
    void nodeDoubleClicked(QtNodes::Node &node);
    // Called when the active output node has started calculations
    void outputComputingStarted();
    // Called when the active output node has finished calculations
    void outputComputingFinished();

signals:
    // When the output node has completed rendering the normal map emit a
    // signal with updated normal and height maps
    void outputUpdated(QImage normal_map, QImage height_map);

private:
    void _updatePropertieNodesShared(QWidget *shared);
    void _updatePropertiesNode(QtNodes::NodeDataModel *node, bool swap = false);
    QtNodes::FlowScene *_scene;
    QtNodes::FlowView *_view;
    // Selected output node to obtain normal and height map from
    QWidget *_properties;
    QWidget *_properties_node = nullptr;
    OutputNode *_active_output = nullptr;
};