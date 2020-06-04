#pragma once

#include <QDialog>
#include <QEvent>
#include <QJsonObject>
#include <QObject>
#include <QPainter>
#include <QPointF>
#include <QWidget>

#include <nodes/NodeDataModel>

#include "../Datatypes/vectormap.h"
#include "Globals/texturelist.h"
#include "node.h"

#include "ui_InputTextureNode.h"
#include "ui_NewTextureDialogue.h"

/**
 * InputTextureNode
 * 
 * A node for having a drawn (created within the program) texture or a loaded
 * texture from the users system.
 */
class InputTextureNode : public Node
{
    Q_OBJECT
    friend class InputTextureNode_Test;

public:
    // Creates the node
    InputTextureNode();

    // When the node is created attach listeners
    void created() override;

    // Title shown at the top of the node
    QString caption() const override;

    // Title shown in the selection list
    QString name() const override;

    // The embedded widget shown on the node
    QWidget *embeddedWidget();

    // The shared widget shown in the properties panel
    QWidget *sharedWidget();

    // Get the number of ports (1 output, 0 input)
    unsigned int nPorts(QtNodes::PortType port_type) const override;

    // Get the port datatype (only exports VectorMapData)
    QtNodes::NodeDataType
    dataType(QtNodes::PortType port_type,
             QtNodes::PortIndex port_index) const override;

    // Get the output data (the VectorMapData)
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port);

    // Save and load the node for project files
    QJsonObject save() const override;
    void restore(QJsonObject const &data) override;

    // Needed for all nodes, even if there are no inputs
    void setInData(std::shared_ptr<QtNodes::NodeData> node_data,
                   QtNodes::PortIndex port);

public slots:
    // Called when the attached texture is updated (drawn on and applied)
    void textureUpdated();

private slots:
    // Select and load a file
    void _loadFile();

    // Create a new square texture
    void _newFileAccept();

private:
    void _setPixmaps();
    // The selected image
    bool _new_file = false;
    int _new_file_res = 1024;

    Texture *_texture = nullptr;
    int _texture_index = -1;

    // The UI elements
    Ui::InputTextureNode _ui;
    Ui::InputTextureNode _shared_ui;
    Ui::NewTextureDialogue _new_texture_ui;
    QWidget *_widget;
    QWidget *_shared;
    QDialog *_dialogue;
};