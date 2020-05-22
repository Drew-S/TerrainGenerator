#pragma once

#include <glm/vec4.hpp>

#include <QWidget>
#include <QObject>

#include <nodes/NodeDataModel>
#include <nodes/Connection>

#include "../Datatypes/pixmap.h"
#include "../Datatypes/vectormap.h"

#include "ui_VectorMathNode.h"

#include "node.h"

class ConverterVectorMathNode : public Node
{
    Q_OBJECT
public:
    enum Mode
    {
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        CROSS
    };
    ConverterVectorMathNode();
    ~ConverterVectorMathNode();

    // When the node is created attach listeners
    void created() override;

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The embedded and shared widgets
    QWidget *embeddedWidget();
    QWidget *sharedWidget();

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

    // Applying function algorithms
    static glm::dvec4 add(glm::dvec4 a, glm::dvec4 b);
    static glm::dvec4 subtract(glm::dvec4 a, glm::dvec4 b);
    static glm::dvec4 multiply(glm::dvec4 a, glm::dvec4 b);
    static glm::dvec4 divide(glm::dvec4 a, glm::dvec4 b);
    static glm::dvec4 cross(glm::dvec4 a, glm::dvec4 b);

public slots:
    // Reset to use constant values when input removed
    void inputConnectionDeleted(QtNodes::Connection const &connection);

    // Input controls
    void x0Changed(double value);
    void y0Changed(double value);
    void z0Changed(double value);

    void x1Changed(double value);
    void y1Changed(double value);
    void z1Changed(double value);

    void modeChanged(int index);

private:
    // Generate the output value
    void _generate();
    // Mixing methods of generation
    void _generateInBoth();
    void _generateIn1(bool second = false);
    void _generateIn();

    // Which mode to apply
    ConverterVectorMathNode::Mode _mode = ConverterVectorMathNode::ADD;

    VectorMap _output{1, 1, glm::dvec4{1.00, 1.00, 1.00, 1.00}};

    // The inputs
    std::shared_ptr<VectorMapData> _in_0;
    std::shared_ptr<VectorMapData> _in_1;

    // The default values to use
    glm::dvec4 _val_in_0{1.00, 1.00, 1.00, 1.00};
    glm::dvec4 _val_in_1{1.00, 1.00, 1.00, 1.00};

    // Whether inputs are set
    bool _in_0_set = false;
    bool _in_1_set = false;

    // UI
    Ui::VectorMathNode _ui;
    Ui::VectorMathNode _shared_ui;
    QWidget *_widget;
    QWidget *_shared_widget;
};