#pragma once

#include <QWidget>
#include <QObject>

#include <nodes/NodeDataModel>
#include <nodes/Connection>

#include "../Datatypes/pixmap.h"

#include "../Datatypes/intensitymap.h"

#include "ui_ClampNode.h"

#include "node.h"

class ConverterClampNode : public Node
{
    Q_OBJECT
    friend class ConverterClampNode_Test;

public:
    enum Mode
    {
        CLAMP,
        SIGMOID
    };
    ConverterClampNode();
    ~ConverterClampNode();

    // When the node is created attach listeners
    void created() override;

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The image label that is embedded in the node
    QWidget *embeddedWidget();
    QWidget *sharedWidget();

    // Get the number of ports (1 output, 3 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only imports IntensityMapData)
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Save and load the node for project files
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Get the output data
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Set the input intensity maps or constant
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port);

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

    std::shared_ptr<IntensityMapData> _input;
    std::shared_ptr<IntensityMapData> _input_min;
    std::shared_ptr<IntensityMapData> _input_max;

    double _min = 0.00;
    double _max = 1.00;

    bool _set = false;
    bool _set_min = false;
    bool _set_max = false;

    Ui::ClampNode _ui;
    Ui::ClampNode _shared_ui;
    QWidget *_widget;
    QWidget *_shared_widget;
};