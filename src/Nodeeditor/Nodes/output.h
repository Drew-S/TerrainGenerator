#pragma once

#include <QLabel>
#include <QImage>
#include <QJsonObject>

#include <nodes/NodeDataModel>
#include <nodes/Connection>

// Normal map generator
#include "./Normal/normal.h"

#include "../Datatypes/pixmap.h"

#include "../Datatypes/intensitymap.h"

#include "node.h"

// Node for managing the final output of the pipeline
// Node that will generated normal map
class OutputNode : public Node
{
public:
    OutputNode();
    ~OutputNode();

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The image label that is embedded in the node
    QWidget *embeddedWidget();

    // Get the number of ports (0 output, 1 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only imports VectorMapData)
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Save and load the node for project files
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Get the output data (nothing)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Set the input pixmap
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port);

    // Get the normal and height maps
    QImage getNormalMap();
    QImage getHeightMap();

public slots:
    void inputConnectionDeleted(QtNodes::Connection const &connection);

private:
    // Generate a normal map (tangent space)
    void _generateNormalMap(IntensityMap height_map);

    // The shared pointer for the inputted pixmap
    std::shared_ptr<IntensityMapData> _input;

    // Generator for the normal map
    NormalMapGenerator _normal_generator;

    // Saved height map image
    QImage _height_map;

    // Houses the generated normal map
    QImage _normal_map;

    // The embedded widget
    QWidget *_widget;

    // Image previews
    QLabel *_height_label;
    QLabel *_normal_label;
};