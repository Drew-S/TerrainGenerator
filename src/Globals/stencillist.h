#pragma once

#include <vector>

#include <QColor>
#include <QPointF>
#include <QLineF>
#include <QPixmap>
#include <QIcon>
#include <QPainter>

#include "../Nodeeditor/Datatypes/intensitymap.h"

// Stencil class loads a monocolour texture for drawing onto pixmaps
class Stencil
{
public:
    // Load from file. Index is reference to location in StencilList below
    Stencil(QString filename, int index);
    // Set the brush size as well
    Stencil(QString filename, double brush, int index);
    // Set the color of the stencil as well
    Stencil(QString filename, double brush, QColor color, int index);
    ~Stencil();

    // Set the color of the stencil
    void setColor(QColor color);

    // Set the opacity of the stencil color
    void setAlpha(double value);

    // Set the brush radius
    void setBrush(double brush);

    // Draw with the painter with position (painter draws to a
    // specific pixmap in its reference)
    void draw(QPainter *painter, QPointF pos);

    // Get the index of this stencil in the stencil list
    int index();

    // Get the name of the stencil "brush-soft.png" -> "soft"
    QString name();

    // Get an icon representation of this stencil
    QIcon icon();

private:
    // Generates pixmap used for the stencil from values
    void _generate();

    // Location in stencil list
    int _index;

    // Filename the stencil is from
    QString _filename;

    // Radius of the brush
    double _brush = 8.00;

    // Color of the brush
    QColor _color{0, 0, 0, 128};

    // Intensity map version of the brush, used for masking
    IntensityMap _stencil;

    // Pixmap representation used for drawing the onto a pixmap
    QPixmap _pixmap;

    // Saved reference of the icon
    QIcon _icon;
};

// Global singleton that keeps a single reference for each stencil
class StencilList
{
public:
    // Get/create a reference to the singleton of stencils
    static StencilList *getInstance();
    ~StencilList();

    // Get a specific stencil with the index
    Stencil *stencil(int index);

    // Get the number of stencils loaded into the program
    int count();

private:
    // Singleton creation values
    StencilList();
    static bool _instance;
    static StencilList *_single;

    // List of the stencils loaded
    std::vector<Stencil> _stencils;
};

#define STENCILS StencilList::getInstance()
