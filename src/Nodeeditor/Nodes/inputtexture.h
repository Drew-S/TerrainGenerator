#pragma once

#include <QLabel>
#include <QObject>
#include <QEvent>

#include <nodeeditor/include/nodes/NodeDataModel>

// Node for the nodeeditor that can load an image from the file system
// Used as an input for other nodes
class InputTextureNode : public QtNodes::NodeDataModel
{
public:
    InputTextureNode();
    ~InputTextureNode(){};

    // Title shown at the top of the node
    QString caption() const override { return "Input Texture"; };

    // Title shown in the selection list
    QString name() const override { return "InputTextureNode"; };
    void name(QString name) { (void)name; };

    // The image label that is embedded in the node
    QWidget *embeddedWidget() { return this->_widget; };

    // Needed for NodeDataModel, not sure where it is used
    QString modelName() { return "Input Texture Node"; };

    // Get the number of ports (1 output, 0 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only exports PixmapData)
    QtNodes::NodeDataType dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const override;

    // Get the output data (the PixmapData)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Needed for all nodes, even if there are no inputs
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
    {
        (void)node_data;
        (void)port;
    };

protected:
    // Used to click on the image to select a texture file
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    // The selected image
    QPixmap _pixmap;

    // The embedded widget
    QLabel *_widget;
};