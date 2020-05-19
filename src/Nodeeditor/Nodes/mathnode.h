#pragma once

#include <QWidget>
#include <QObject>

#include <nodes/NodeDataModel>
#include <nodes/Connection>

#include "../Datatypes/pixmap.h"

#include "../Datatypes/intensitymap.h"

#include "ui_MathNode.h"

// Apply a mathematical function per pixel to an intensity map (map/map) (map/value) (value/value)
class ConverterMathNode : public QtNodes::NodeDataModel
{
    Q_OBJECT
public:
    // Which algorithm to use when combining maps/values
    enum Mode
    {
        MIX,
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        MIN,
        MAX,
        POW
    };
    ConverterMathNode();
    ~ConverterMathNode();

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

    // Get the number of ports (1 output, 2 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only imports VectorMapData)
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Save and load the node for project files
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Get the output data
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Set the input intensity maps or constant
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port);

    // Transformation algorithms
    static double mix(double a, double b);
    static double add(double a, double b);
    static double subtract(double a, double b);
    static double multiply(double a, double b);
    static double divide(double a, double b);
    static double min(double a, double b);
    static double max(double a, double b);
    static double pow(double a, double b);

public slots:
    // Reset to use constant values when input removed
    void inputConnectionDeleted(QtNodes::Connection const &connection);

    void val0Changed(double value);
    void val1Changed(double value);
    void comboChanged(int index);

private:
    // Generate the output pixmap
    void _gen();
    // Mixing methods of generation
    void _gen_in_both();                 // both set
    void _gen_in_1(bool second = false); // 1 set
    void _gen_in();                      // none set

    // The algorithm to use for mixing
    ConverterMathNode::Mode _mode = ConverterMathNode::MIX;

    // The results
    IntensityMap _pixmap;

    // Input maps for input ports
    std::shared_ptr<IntensityMapData> _in_0;
    std::shared_ptr<IntensityMapData> _in_1;

    // Input values when ports are not used
    double _val_in_0 = 1.00;
    double _val_in_1 = 1.00;

    // Whether or not the ports are set
    bool _in_0_set = false;
    bool _in_1_set = false;

    // UI widgets
    Ui::MathNode _ui;
    Ui::MathNode _shared_ui;
    QWidget *_widget;
    QWidget *_shared_widget;
};