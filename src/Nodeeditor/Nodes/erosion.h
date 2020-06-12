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

    void waterIncrement1();
    void flowSimulation2();
    void erosionAndDeposition3();
    void sedimentTransportation4();
    void evaporation5();

    void smooth();

public slots:
    void inputConnectionDeleted(QtNodes::Connection const &connection);

private:
    void _generate();
    IntensityMap _output{1, 1, 1.00}; // Terrain b
    IntensityMap _sediment{1, 1, 0.00}; // Terrain b
    IntensityMap _erosion{1, 1, 0.00}; // Terrain b
    
    IntensityMap w{1, 1, 0.00}; // Water d
    IntensityMap w_1{1, 1, 0.00}; // Water d1
    IntensityMap w_2{1, 1, 0.00}; // Water d1
    IntensityMap s{1, 1, 0.00}; // Sediment s
    IntensityMap s_1{1, 1, 0.00}; // Sediment s1
    //                           L     R     T     B
    VectorMap f{1, 1, glm::dvec4{0.00, 0.00, 0.00, 0.00}}; // Outflow flux f
    //                           u     v
    VectorMap v{1, 1, glm::dvec4{0.00, 0.00, 0.00, 0.00}}; // Velocity v (x, y only)
    IntensityMap r{1, 1, 0.00}; // Water source r

    std::shared_ptr<IntensityMapData> _input;

    bool _set = false;

    // CONSTANTS
    double A = 0.00005; // Virtual pipe area
    double L = 1.00; // Virtual pipe length
    double g = 9.81; // Gravity constant
    double K_c = 25.00; // Sediment capacity constant
    double K_s = 0.012; // Sediment dissolving constant
    double K_d = 0.012; // Sediment depositing constant
    double K_e = 0.000055; // Evaporation constant
    double dT = 16.6666666666666666666666666; // Change in time
    double dX = 2.00; // Change in grid distance (x)
    double dY = 2.00; // Change in grid distance (y)
    double p = 1.00;
    int I = 1;

    glm::dvec3 n{0.00, 0.00, 1.00};

    Ui::Erosion _ui;
    QWidget *_widget;
};