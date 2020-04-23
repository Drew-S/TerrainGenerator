#pragma once

#include <QLabel>
#include <QImage>

#include <nodeeditor/include/nodes/NodeDataModel>

// Node for managing the final output of the pipeline
// Generates the normal map
class OutputNode : public QtNodes::NodeDataModel
{
public:
    OutputNode();
    ~OutputNode(){};

    // Title shown at the top of the node
    QString caption() const override { return QString("Output"); };

    // Title shown in the selection list
    QString name() const override { return QString("OutputNode"); };
    void name(QString name) { (void)name; };

    // The image label that is embedded in the node
    QWidget *embeddedWidget() { return this->_widget; };

    // Needed for NodeDataModel, not sure where it is used
    QString modelName() { return QString("Output Node"); };

    // Get the number of ports (0 output, 1 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only imports PixmapData)
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Get the output data (nothing)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port)
    {
        (void)port;
        return nullptr;
    };

    // Set the input pixmap
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port);

    // Get the normal and height maps
    QImage getNormalMap();
    QImage getHeightMap();

private:
    // Generate a normal map (tangent space)
    void generateNormalMap();

    // The shared pointer for the inputted pixmap
    std::shared_ptr<QtNodes::NodeData> _pixmap;

    // Houses the generated normal map
    QImage _normal_map;

    // The embedded widget
    QLabel *_widget;
};