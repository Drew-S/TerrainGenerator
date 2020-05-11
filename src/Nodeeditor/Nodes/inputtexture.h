#pragma once

#include <QWidget>
#include <QObject>
#include <QEvent>
#include <QJsonObject>
#include <QDialog>
#include <QPainter>
#include <QPointF>

#include <nodes/NodeDataModel>

#include "../Datatypes/vectormap.h"

#include "./Stencil/stencil.h"
#include "./Stencil/stencillist.h"

#include "../global.h"

#include "ui_InputTextureNode.h"
#include "ui_NewTextureDialogue.h"
#include "ui_InputDrawingNode.h"

// Node for the nodeeditor that can load an image from the file system
// Used as an input for other nodes
class InputTextureNode : public QtNodes::NodeDataModel
{
    Q_OBJECT
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
    QWidget *sharedWidget();

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
    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void _loadFile();
    void _newFileAccept();

private:
    void _generate();
    void _setPixmaps();
    // The selected image
    bool _new_file = false;
    int _new_file_res = 1024;
    QPixmap *_pixmap;
    QString _filename = "";
    VectorMap _color_map{0, 0};

    double _brush = 8.00;
    double _flow = 99.0;
    QColor _color;

    StencilList *_stencils;
    QPainter *_painter;

    bool _prev_first = false;
    QPointF _prev;

    // The embedded widget
    Ui::InputTextureNode _ui;
    Ui::NewTextureDialogue _new_texture_ui;
    Ui::InputDrawingNodeWidget _drawing_ui;
    QWidget *_widget;
    QWidget *_shared;
    QDialog *_dialogue;
    GlobalData *_global;
};