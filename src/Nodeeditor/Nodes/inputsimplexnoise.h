#pragma once

#include <QJsonObject>
#include <QObject>
#include <QPixmap>
#include <QThread>
#include <QVector3D>
#include <QWidget>

#include "SimplexNoise.h"
#include <nodes/Connection>
#include <nodes/NodeDataModel>

#include "../Datatypes/intensitymap.h"
#include "../Datatypes/pixmap.h"
#include "../Datatypes/vectormap.h"

#include "node.h"

#include "ui_SimplexNoiseNode_no_scroll.h"
#include "ui_SimplexNoiseNode.h"

/**
 * SimlpexNoiseWorker
 * 
 * Worker class for running the simplex noise generation within its own separate
 * thread.
 */
class SimplexNoiseWorker : public QObject
{
    Q_OBJECT
public:
    // Set any data that is needed, (must be in other thread first)
    void set(float octives,
             float frequency,
             float persistence,
             QVector3D offset,
             IntensityMap *height_map);

public slots:
    // Run generation
    void generate();

    // Stop generation
    void stop();

signals:
    // Updating signals
    void started();
    void progress(int perc);
    void done();
    void stopped();

private:
    // Generation parameters
    float _octives = 8.0f;
    float _frequency = 5.0f;
    float _persistence = 0.5f;
    QVector3D _offset{0.0f, 0.0f, 0.0f};

    // Resulting height map
    IntensityMap *_height_map;

    // Flag for interuption
    bool _run = false;
};

/**
 * InputSimplexNoiseNode
 * 
 * Input node for creating simplex noise textures.
 */
class InputSimplexNoiseNode : public Node
{
    Q_OBJECT
    friend class InputSimplexNoiseNode_Test;

public:
    // Creates the node
    InputSimplexNoiseNode();
    ~InputSimplexNoiseNode();

    // When the node is created attach listeners
    void created() override;

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The image label that is embedded in the node
    QWidget *embeddedWidget();
    QWidget *sharedWidget();

    // Get the number of ports (1 output, 0 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only exports VectorMapData)
    QtNodes::NodeDataType
    dataType(QtNodes::PortType port_type,
             QtNodes::PortIndex port_index) const override;

    // Get the output data (the VectorMapData)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Save and restore the nodes state
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Needed for all nodes, even if there are no inputs
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data,
                   QtNodes::PortIndex port);

public slots:
    // Reset to use constant values when input removed
    void inputConnectionDeleted(QtNodes::Connection const &connection);
    void simplexDone();

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
    void _generate();
    // Houses the pixmap to be passed
    IntensityMap _intensity_map;
    QPixmap _output;

    // Generator for open simplex noise
    SimplexNoise _noise;

    // Adjustment parameters
    float _octives = 8.0f;
    float _frequency = 5.0f;
    float _persistence = 0.5f;
    QVector3D _offset{0.0f, 0.0f, 0.0f};

    QThread _thread;
    SimplexNoiseWorker *_worker = nullptr;

    // Inputs
    std::shared_ptr<IntensityMapData> _in_octives;
    std::shared_ptr<IntensityMapData> _in_frequency;
    std::shared_ptr<IntensityMapData> _in_persistence;
    std::shared_ptr<VectorMapData> _in_offset;

    // Whether inputs are set
    bool _in_octives_set = false;
    bool _in_frequency_set = false;
    bool _in_persistence_set = false;
    bool _in_offset_set = false;

    // Housing widget and ui
    QWidget *_widget;
    QWidget *_shared_widget;
    Ui::SimplexNoiseNode_no_scroll _ui;
    Ui::SimplexNoiseNode _shared_ui;
};