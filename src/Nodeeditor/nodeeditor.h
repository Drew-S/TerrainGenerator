#pragma once

#include <QLayout>
#include <QObject>

#include <nodeeditor/include/nodes/FlowScene>
#include <nodeeditor/include/nodes/FlowView>
#include <nodeeditor/include/nodes/Node>

#include "Nodes/output.h"
#include "Nodes/inputtexture.h"

// Wrapper to manage the FlowView and FlowScene and listen for events
class Nodeeditor : public QObject
{
    Q_OBJECT

public:
    Nodeeditor(QLayout *target);
    ~Nodeeditor();

    // Returns the height/normal map of the active selected output node
    QImage getHeightMap();
    QImage getNormalMap();

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
    QtNodes::FlowScene *_scene;
    QtNodes::FlowView *_view;
    // Selected output node to obtain normal and height map from
    OutputNode *_active_output;
};