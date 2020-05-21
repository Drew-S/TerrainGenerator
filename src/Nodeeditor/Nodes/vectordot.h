#pragma once

#include "../Datatypes/intensitymap.h"
#include "../Datatypes/vectormap.h"
#include "../Datatypes/pixmap.h"

#include <nodes/NodeDataModel>
#include <nodes/Connection>

#include <QObject>
#include <QJsonObject>

#include <glm/vec4.hpp>

#include "ui_VectorDotProduct.h"

class ConverterVectorDotNode : public QtNodes::NodeDataModel
{
    Q_OBJECT
public:
    ConverterVectorDotNode();
    ~ConverterVectorDotNode();

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The image label that is embedded in the node
    QWidget *embeddedWidget();
    QWidget *sharedWidget();

    // Get the number of ports (1 output, 2 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Get the output data (the IntensityMapData)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Save and load the node
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Needed for all nodes, even if there are no inputs
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port_index);

public slots:
    // Reset to use constant values when input removed
    void inputConnectionDeleted(QtNodes::Connection const &connection);

    // First vector changed
    void x0Changed(double value);
    void y0Changed(double value);
    void z0Changed(double value);
    void w0Changed(double value);

    // Second vector changed
    void x1Changed(double value);
    void y1Changed(double value);
    void z1Changed(double value);
    void w1Changed(double value);

private:
    // Generate results
    void _generate();
    void _generateInBoth();                 // Both set
    void _generateIn1(bool second = false); // 1 set
    void _generateIn();                     // None set

    // Get the dot product of two vectors
    static double dot(glm::dvec4 a, glm::dvec4 b);

    // Results
    IntensityMap _pixmap{1, 1, 1.00};

    // Input values
    std::shared_ptr<VectorMapData> _in_0;
    std::shared_ptr<VectorMapData> _in_1;

    // Default solid input values
    glm::dvec4 _in_val_0{1.00, 1.00, 1.00, 1.00};
    glm::dvec4 _in_val_1{1.00, 1.00, 1.00, 1.00};

    // Whether input values are set
    bool _in_set_0 = false;
    bool _in_set_1 = false;

    // UI elements
    Ui::VectorDotProduct _ui;
    Ui::VectorDotProduct _shared_ui;
    QWidget *_widget;
    QWidget *_shared_widget;
};