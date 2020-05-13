#pragma once

#include "ui_TextureDrawing.h"

#include "Globals/stencillist.h"
#include "Globals/texturelist.h"

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPointF>

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

protected:
    // Controls drawing and zooming
    bool eventFilter(QObject *object, QEvent *event);

private:
    // Drawing is a singleton that can be referenced by multiple texturenodes
    DrawingDialogue();
    static bool _instance;
    static DrawingDialogue *_single;

    // How regularly do we draw the stencil
    double _flow_rate = 99.00;

    // The stencil to draw with
    Stencil *_active_stencil = nullptr;

    // The texture to draw to
    Texture *_active_texture = nullptr;

    // Dialogue setup
    QDialog _dialogue;
    Ui::TextureDrawingDialogue _ui;

    // Scene for drawing the texture map onto
    QGraphicsScene *_scene;
    // Reference to pixmap to update visuals
    QGraphicsPixmapItem *_pixmap;
    // Used for drawing distance and flow comparison
    QPointF _prev;
};

#define DRAWING DrawingDialogue::getInstance()
