#pragma once

#include <vector>

#include "ui_TextureDrawing.h"

#include "Globals/stencillist.h"
#include "Globals/texturelist.h"

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPointF>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QPushButton>
#include <QColor>

// Dialogue used for manipulating (drawing) on existing textures
class DrawingDialogue : public QObject
{
    Q_OBJECT
public:
    static DrawingDialogue *getInstance();
    ~DrawingDialogue();

    // Show the dialogue, using existing selected texture
    // defaults to 0
    void show();

    // Show the dialogue with a specific texture (index on the texturelist)
    void show(int index);

public slots:
    // Update pixmap as the user draws
    void textureUpdated();
    void newTexture(int index);

protected:
    // Controls drawing and zooming
    bool eventFilter(QObject *object, QEvent *event);

private:
    // Drawing is a singleton that can be referenced by multiple texturenodes
    DrawingDialogue();
    static bool _instance;
    static DrawingDialogue *_single;

    // How regularly do we draw the stencil
    double _brush = 8.00;
    double _opacity = 100.00;
    double _flow_rate = 99.00;
    QColor _color{0, 0, 0, 255};

    // The stencil to draw with
    Stencil *_active_stencil = nullptr;
    int _active_stencil_button = 0;
    std::vector<QPushButton *> _stencil_buttons;

    // The texture to draw to
    Texture *_original_texture = nullptr;

    // Dialogue setup
    QDialog _dialogue;
    Ui::TextureDrawingDialogue _ui;

    // Scene for drawing the texture map onto
    QGraphicsScene *_scene;
    // Reference to pixmap to update visuals
    QGraphicsPixmapItem *_pixmap;
    // Used for drawing distance and flow comparison
    QPointF _prev;
    QStandardItemModel _texture_list_model;
    QModelIndex _selected_model;
};

#define DRAWING DrawingDialogue::getInstance()
