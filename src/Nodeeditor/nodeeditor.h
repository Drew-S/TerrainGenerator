#pragma once

#include <QJsonObject>
#include <QLayout>
#include <QObject>
#include <QWidget>

#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/Node>

// Output node
#include "./Nodes/output.h"

// Input nodes
#include "./Nodes/constantvalue.h"
#include "./Nodes/constantvector.h"
#include "./Nodes/inputsimplexnoise.h"
#include "./Nodes/inputtexture.h"

// Converter Nodes
#include "./Nodes/bezier.h"
#include "./Nodes/clamp.h"
#include "./Nodes/colorcombine.h"
#include "./Nodes/colorsplit.h"
#include "./Nodes/invertintensity.h"
#include "./Nodes/math.h"
#include "./Nodes/normalize.h"
#include "./Nodes/vectordot.h"
#include "./Nodes/vectorintensity.h"
#include "./Nodes/vectormath.h"
#include "./Nodes/erosion.h"
#include "./Nodes/smooth.h"

/**
 * Nodeeditor
 * 
 * Class that wraps QtNodes flow view and flow scene
 * (QGraphicsView/QGraphicsScene) and manages the nodes and signals.
 */
class Nodeeditor : public QObject
{
    Q_OBJECT

public:
    // Constructor that creates the scene and view and attaches to the UI.
    Nodeeditor(QLayout *target, QWidget *properties);
    ~Nodeeditor();

    // Returns the height/normal map of the active selected output node
    QImage getHeightMap();
    QImage getNormalMap();
    QImage getAlbedoMap();

    // Save/load the editor nodes, layout, and connections
    QJsonObject save();
    void load(QJsonObject data);

public slots:
    // Called when a node is created
    void nodeCreated(QtNodes::Node &node);
    // Called when a node is double clicked (selected as primary)
    void nodeDoubleClicked(QtNodes::Node &node);
    // Called when the active output node has finished calculations
    void outputComputingFinished();
    // Called when a node is deleted, update properties panel if necessary
    void nodeDeleted(QtNodes::Node &node);

signals:
    // When the output node has completed rendering the normal map emit a
    // signal with updated normal and height maps
    void outputUpdated(QImage normal_map, QImage height_map, QImage albedo_map);

private:
    // Sets the properties widget.
    void _updatePropertieNodesShared(QWidget *shared);

    // Sets the properties widget from the selected node.
    void _updatePropertiesNode(QtNodes::NodeDataModel *node, bool swap = false);

    // Managed UI
    QtNodes::FlowScene *_scene = nullptr;
    QtNodes::FlowView *_view = nullptr;

    // Container for the properties panel (duplicate the node embeddedWidget).
    QWidget *_properties;
    QWidget *_properties_node = nullptr;

    // Selected output node to obtain normal and height map from.
    OutputNode *_active_output = nullptr;
};