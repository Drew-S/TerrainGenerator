/**
 * Erosion Node is not quiet accurate enough
 * 
 * NOTE: The flow of the water appears to be correct
 * NOTE: The erosion of the soil appears to be correct
 * NOTE: The sediment of the soil appears to be incorrect FIXME: Incorrect sedimentation
 * NOTE: Evaporation is wrong? FIXME: See above, may be connected.
 * 
 */
#pragma once

#include <QObject>
#include <QWidget>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <nodes/Connection>
#include <nodes/NodeDataModel>

#include "../Datatypes/intensitymap.h"
#include "../Datatypes/vectormap.h"
#include "../Datatypes/pixmap.h"
#include "node.h"

#include "ui_Erosion.h"

/**
 * ConverterErosionNode
 * 
 * Applies erosion simulation (water and rain) over the provided terrain height
 * map.
 */
class ConverterErosionNode : public Node
{
    Q_OBJECT
public:
    // Create the node
    ConverterErosionNode();

    // When the node is created attach listeners
    void created() override;

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The embedded widget shown in the node
    QWidget *embeddedWidget();

    // The shared widget shown in the properties panel
    QWidget *sharedWidget();

    // Get the number of ports (3 output, 1 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only imports VectorMapData)
    QtNodes::NodeDataType
    dataType(QtNodes::PortType port_type,
             QtNodes::PortIndex port_index) const override;

    // Save and load the node for project files
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Get the output data
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Set the input intensity maps or constant
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data,
                   QtNodes::PortIndex port);

public slots:
    void inputConnectionDeleted(QtNodes::Connection const &connection);

private:
    void _generate();
    void _smooth(int x, int y);
    IntensityMap _output{1, 1, 1.00};   // Terrain b
    IntensityMap _sediment{1, 1, 0.00}; // Terrain b
    IntensityMap _erosion{1, 1, 0.00}; // Terrain b

    std::shared_ptr<IntensityMapData> _input;

    bool _set = false;

    int _iterations = 10000;
    int _max_drop_life = 200;

    double _inertia = 0.4; // Inertia constant
    double _sediment_capacity = 1.00; // Sediment capacity constant
    double _min_sediment_capacity = 0.01; // Minimum sediment capacity constant
    double _deposit_speed = 0.012; // Deposit speed constant
    double _erosion_speed = 0.012; // Erode speed constant
    double _erosion_radius = 0.2; // Erosion radius
    double _g = 4.00;
    double _evaporation_rate = 0.12;
    double _smooth_strength = 0.10;

    Ui::Erosion _ui;
    QWidget *_widget;
};