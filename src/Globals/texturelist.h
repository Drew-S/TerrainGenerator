#pragma once

#include <vector>

#include <QByteArray>
#include <QIcon>
#include <QImage>
#include <QJsonObject>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QPointF>

#include "../Nodeeditor/Datatypes/intensitymap.h"
#include "../Nodeeditor/Datatypes/vectormap.h"
#include "Globals/stencillist.h"

/**
 * Texture
 * 
 * The texture class houses loaded and generated textures that are provided in
 * the InputTextureNode class and edited with the drawing singleton.
 */
class Texture : public QObject
{
    Q_OBJECT
public:
    // Create textures
    Texture();
    // From file
    Texture(QString filename);
    // Load from packed file
    Texture(QByteArray data, QString filename);
    // With size and default colour (white)
    Texture(int w, int h);
    // With size and a filename
    Texture(int w, int h, QString filename);

    // Convert to pixmap
    QPixmap pixmap(int scale = -1);

    // Convert to image
    QImage image(int scale = -1);

    // Convert to icon
    QIcon icon();

    // For saving internally in zip
    bool bytes(QByteArray *bytes);

    // Flags as to whether the texture is generated (pack into saves)
    // and/or edited (ask to save changes before exiting for external resources)
    bool generated();
    bool edited();

    // Texture name's
    QString name();
    QString filename();
    QString saveName();

    // Convert to vector map
    VectorMap vectorMap(int scale = -1);

    // Replace the underlying pixmap
    void replace(QPixmap pixmap);

    // Convert to intensity map
    IntensityMap intensityMap(IntensityMap::Channel channel, int scale = -1);

    // Draw on the texture with the stencil at a specific point,
    // if update emit updated() signal
    void draw(Stencil *stencil, QPointF pos);

    // Save the texture with the existing filename
    bool save();
    bool save(QIODevice *device);

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

    bool _generated = false;
    bool _edited = false;
};

/**
 * TextureList
 * 
 * Singleton class that houses the entire list of textures used throughout the
 * program. InputTextureNodes that create new textures do so through this
 * singleton. The drawing singleton lists active textures by polling this.
 */
class TextureList : public QObject
{
    Q_OBJECT
public:
    // Get/create reference to TextureList singleton
    static TextureList *getInstance();
    ~TextureList();

    // Add a texture to the list (from file)
    int addTexture(QString filename);
    int loadTexture(QByteArray data, QString filename);

    // Add a new texture to the list
    int addTexture(int w, int h);

    // Get the number of textures
    int count();

    // Get the reference to a specific texture,
    // returns nullptr if beyond bounds
    Texture *at(int index);

    // Find a loaded texture using its filename
    int find(QString filename);

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