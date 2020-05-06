#pragma once

#include "lib/SimplexNoise/src/SimplexNoise.h"

#include "../Datatypes/intensitymap.h"

#include "ui_SimplexNoiseNode.h"
#include "ui_SimplexNoiseNode_no_scroll.h"

#include <nodes/NodeDataModel>

#include <QPixmap>
#include <QWidget>
#include <QObject>
#include <QVector3D>
#include <QJsonObject>

// Input Node for generating simplex noise maps
class InputSimplexNoiseNode : public QtNodes::NodeDataModel
{
    Q_OBJECT
public:
    InputSimplexNoiseNode();
    ~InputSimplexNoiseNode();

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

    // Get the number of ports (1 output, 0 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only exports VectorMapData)
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Get the output data (the VectorMapData)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Needed for all nodes, even if there are no inputs
    // TODO: Add support for input control parameters
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port);

private slots:
    // Watches for changes in the widget to update noise parameters
    void octivesChanged(double value);
    void frequencyChanged(double value);
    void persistenceChanged(double value);
    void xChanged(double value);
    void yChanged(double value);
    void zChanged(double value);

private:
    // Generates a pixmap
    void
    _generate();
    // Houses the pixmap to be passed
    IntensityMap _intensity_map;
    QPixmap _pixmap;

    // Generator for open simplex noise
    SimplexNoise _noise;

    // Adjustment parameters
    float _octives = 8.0f;
    float _frequency = 5.0f;
    float _persistence = 0.5f;
    QVector3D _offset{0.0f, 0.0f, 0.0f};

    // Housing widget and ui
    QWidget *_widget;
    QWidget *_shared_widget;
    Ui::SimplexNoiseNode_no_scroll _ui;
    Ui::SimplexNoiseNode _shared_ui;
};