#pragma once

#include <QObject>
#include <QWidget>

#include <nodes/Connection>
#include <nodes/NodeDataModel>

#include "../Datatypes/intensitymap.h"
#include "../Datatypes/pixmap.h"
#include "node.h"

#include "ui_ClampNode.h"

/**
 * ConverterClampNode
 * 
 * Node that clamps an input intensity map by restricting the range of values
 * between a MIN and MAX value. The user can select either a hard clamp that
 * truncates values beyond the bounds of the range or a smooth clamp using a
 * sigmoid function.
 */
class ConverterClampNode : public Node
{
    Q_OBJECT
    friend class ConverterClampNode_Test;

public:
    // What method to use when clamping the data
    enum Mode
    {
        CLAMP,
        SIGMOID
    };

    // Create the node
    ConverterClampNode();

    // When the node is created attach listeners
    void created() override;

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // Embedded widget shown within the node
    QWidget *embeddedWidget();

    // The shared widget shown in the properties panel
    QWidget *sharedWidget();

    // Get the number of ports (1 output, 3 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only imports IntensityMapData)
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

    // clamping functions, hard clamp or sigmoid
    static double clamp(double v, double min, double max);
    static double sigmoid(double v, double min, double max);

public slots:
    // Reset to use constant values when input removed
    void inputConnectionDeleted(QtNodes::Connection const &connection);

private:
    // Generate the output pixmap
    void _generate();

    IntensityMap _output{1, 1, 1.00};

    ConverterClampNode::Mode _mode = ConverterClampNode::CLAMP;

    // Input data pointers
    std::shared_ptr<IntensityMapData> _input;
    std::shared_ptr<IntensityMapData> _input_min;
    std::shared_ptr<IntensityMapData> _input_max;

    // Default values for min and max (if input not set)
    double _min = 0.00;
    double _max = 1.00;

    // Flags to track what inputs are set
    bool _set = false;
    bool _set_min = false;
    bool _set_max = false;

    // UI elements
    Ui::ClampNode _ui;
    Ui::ClampNode _shared_ui;
    QWidget *_widget;
    QWidget *_shared_widget;
};