#pragma once

#include "../Datatypes/intensitymap.h"
#include "../Datatypes/pixmap.h"

#include <nodes/NodeDataModel>
#include <nodes/Connection>

#include <QObject>
#include <QJsonObject>

// Node that converts a VectorMap input and splits the colour channels out into
// individual IntensityMap for rgba
class ConverterColorSplit : public QtNodes::NodeDataModel
{
    Q_OBJECT

public:
    ConverterColorSplit();
    ~ConverterColorSplit();

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;
    void name(QString name);

    // The image label that is embedded in the node
    QWidget *embeddedWidget();

    // Needed for NodeDataModel, not sure where it is used
    QString modelName();

    // Get the number of ports (4 output, 1 input)
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

private:
    // Splits the vector map to individual channels
    void _generate();

    // Individual channels for the output
    IntensityMap _red{1, 1, 1.00};
    IntensityMap _green{1, 1, 1.00};
    IntensityMap _blue{1, 1, 1.00};
    IntensityMap _alpha{1, 1, 1.00};

    std::shared_ptr<VectorMapData> _input;
    bool _set = false;
};
