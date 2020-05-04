#pragma once

#include <QLabel>
#include <QObject>
#include <QEvent>
#include <QJsonObject>

#include <nodes/NodeDataModel>

#include "../Datatypes/vectormap.h"

// Node for the nodeeditor that can load an image from the file system
// Used as an input for other nodes
class InputTextureNode : public QtNodes::NodeDataModel
{
public:
    InputTextureNode();
    ~InputTextureNode();

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;
    void name(QString name);

    // The image label that is embedded in the node
    QWidget *embeddedWidget();

    // Needed for NodeDataModel, not sure where it is used
    QString modelName();

    // Get the number of ports (1 output, 0 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only exports VectorMapData)
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Get the output data (the VectorMapData)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Save and load the node for project files
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Needed for all nodes, even if there are no inputs
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port);

protected:
    // Used to click on the image to select a texture file
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    void _generate();
    // The selected image
    QPixmap _pixmap;
    QString _filename = "";
    VectorMap _color_map;

    // The embedded widget
    QLabel *_widget;
};