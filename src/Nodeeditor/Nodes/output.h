#pragma once

#include <QLabel>
#include <QImage>
#include <QJsonObject>

#include <nodes/NodeDataModel>
#include <nodes/Connection>

#include "../Datatypes/intensitymap.h"
#include "../Datatypes/pixmap.h"
#include "./Normal/normal.h"
#include "node.h"

#include "ui_Output.h"

/**
 * OutputNode
 * 
 * The final node that takes the generated height map, generates a normal map,
 * and informs to display the changes.
 */
class OutputNode : public Node
{
    Q_OBJECT
public:
    // Create the node
    OutputNode();

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The embedded widget in the node
    QWidget *embeddedWidget();

    // Get the number of ports (0 output, 2 input)
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
    QImage getAlbedoMap();

public slots:
    void inputConnectionDeleted(QtNodes::Connection const &connection);

private:
    // Generate a normal map (tangent space)
    void _generateNormalMap(IntensityMap height_map);

    // The shared pointer for the inputted pixmap
    std::shared_ptr<IntensityMapData> _input;
    std::shared_ptr<VectorMapData> _input_albedo;

    // Generator for the normal map
    NormalMapGenerator _normal_generator;

    // Saved height map image
    QImage _height_map;

    // Houses the generated normal map
    QImage _normal_map;

    QImage _albedo_map;

    // The embedded widget
    Ui::Output _ui;
    QWidget *_widget;
};