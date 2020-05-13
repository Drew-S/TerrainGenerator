#pragma once

#include <vector>

#include <QObject>
#include <QPixmap>
#include <QImage>
#include <QIcon>
#include <QPainter>
#include <QPointF>

#include "Globals/stencillist.h"
#include "../Nodeeditor/Datatypes/vectormap.h"
#include "../Nodeeditor/Datatypes/intensitymap.h"

// Texture class houses pixmap that can be drawn on,
// used to share textures with pointer (multiple texture
// nodes or the drawing dialogue)
class Texture : public QObject
{
    Q_OBJECT
public:
    // Create textures
    Texture();
    // From file
    Texture(QString filename);
    // With size and default colour (white)
    Texture(int w, int h);
    // With size and a filename
    Texture(int w, int h, QString filename);
    ~Texture();

    // Convert to pixmap
    QPixmap pixmap();

    // Convert to image
    QImage image();

    // Convert to icon
    QIcon icon();

    // Texture name
    QString name();

    // Convert to vector map
    VectorMap vectorMap();

    // Replace the underlying pixmap
    void replace(QPixmap pixmap);

    // Convert to intensity map
    IntensityMap intensityMap(IntensityMap::Channel channel);

    // Draw on the texture with the stencil at a specific point,
    // if update emit updated() signal
    void draw(Stencil *stencil, QPointF pos, bool update = false);

    // Call updated signal
    void apply();

    // Save the texture with the existing filename
    bool save();

    // Overright save location
    bool saveAs(QString filename);

signals:
    // Called after drawing if update true
    void updated();

private:
    QString _filename = "";
    QPixmap _pixmap;
    // Painter for drawing on the _pixmap
    QPainter *_painter;
};

// Global singleton that manages all the Texture's provided in the system
// This is used to allow nodes to select existing loaded textures from
// as central location rather than separate management. Makes draw management
// easier and allows for multiple nodes to reference the same texture without
// duplicating data
class TextureList : public QObject
{
    Q_OBJECT
public:
    // Get/create reference to TextureList singleton
    static TextureList *getInstance();
    ~TextureList();

    // Add a texture to the list (from file)
    int addTexture(QString filename);

    // Add a new texture to the list
    int addTexture(int w, int h);

    // Get the number of textures
    int count();

    // Get the reference to a specific texture,
    // returns nullptr if beyond bounds
    Texture *at(int index);

signals:
    // Called when a new texture is added
    void textureAdded(int index);

private:
    TextureList();
    // The list of referenced textures
    std::vector<Texture *> _textures;
    static bool _instance;
    static TextureList *_single;
    static int _num;
};

#define TEXTURES TextureList::getInstance()