#pragma once

#include "lib/SimplexNoise/src/SimplexNoise.h"

#include "../Datatypes/structures.h"

#include "ui_SimplexNoise.h"

#include <lib/nodeeditor/include/nodes/NodeDataModel>

#include <QPixmap>
#include <QWidget>
#include <QObject>
#include <QVector3D>

// Input Node for generating simplex noise maps
class InputSimplexNoiseNode : public QtNodes::NodeDataModel
{
    Q_OBJECT
public:
    InputSimplexNoiseNode();
    ~InputSimplexNoiseNode(){};

    // Title shown at the top of the node
    QString caption() const override { return "Simplex Noise Texture"; };

    // Title shown in the selection list
    QString name() const override { return "InputSimplexNoiseNode"; };
    void name(QString name) { (void)name; };

    // The image label that is embedded in the node
    QWidget *embeddedWidget() { return this->_widget; };

    // Needed for NodeDataModel, not sure where it is used
    QString modelName() { return "Input Simplex Noise Node"; };

    // Get the number of ports (1 output, 0 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only exports PixmapData)
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Get the output data (the PixmapData)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Needed for all nodes, even if there are no inputs
    // TODO: Add support for input control parameters
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
    {
        (void)node_data;
        (void)port;
    };

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
    VectorMap _color_map;
    QPixmap _pixmap;

    // Generator for open simplex noise
    SimplexNoise _noise;

    // Adjustment parameters
    float _octives = 1.0f;
    float _frequency = 1.0f;
    float _persistence = 0.5f;
    QVector3D _offset{0.0f, 0.0f, 0.0f};

    // Housing widget and ui
    QWidget *_widget;
    Ui::SimplexNoiseWidget _ui;
};