#pragma once

#include "../Datatypes/intensitymap.h"
#include "../Datatypes/pixmap.h"

#include "ui_ColorCombineNode.h"

#include <nodes/NodeDataModel>
#include <nodes/Connection>

#include <QObject>
#include <QJsonObject>

#include <glm/vec4.hpp>

class ConverterColorCombine : public QtNodes::NodeDataModel
{
    Q_OBJECT

public:
    ConverterColorCombine();
    ~ConverterColorCombine();

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;
    void name(QString name);

    // The embedded control widget
    QWidget *embeddedWidget();
    QWidget *sharedWidget();

    // Needed for NodeDataModel, not sure where it is used
    QString modelName();

    // Get the number of ports (1 output, 4 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Get the output data (the VectorMapData)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Save and load the node
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Needed for all nodes, even if there are no inputs
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port);

public slots:
    // Reset to use constant values when input removed
    void inputConnectionDeleted(QtNodes::Connection const &connection);

    // When ui changes update
    void redChanged(double value);
    void greenChanged(double value);
    void blueChanged(double value);
    void alphaChanged(double value);

private:
    // Generate the output
    void _generate();

    // The resulting output (defaults to solid white)
    VectorMap _pixmap{1, 1, glm::dvec4{1.00, 1.00, 1.00, 1.00}};

    // Input intensity maps for different colors
    std::shared_ptr<IntensityMapData> _red;
    std::shared_ptr<IntensityMapData> _green;
    std::shared_ptr<IntensityMapData> _blue;
    std::shared_ptr<IntensityMapData> _alpha;

    // Default solid colours
    double _red_val = 1.00;
    double _green_val = 1.00;
    double _blue_val = 1.00;
    double _alpha_val = 1.00;

    // Whether or not certain inputs are set
    bool _red_set = false;
    bool _green_set = false;
    bool _blue_set = false;
    bool _alpha_set = false;

    Ui::ColorCombineNode _ui;
    Ui::ColorCombineNode _shared_ui;
    QWidget *_widget;
    QWidget *_shared_widget;
};