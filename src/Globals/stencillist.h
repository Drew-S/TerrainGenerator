#pragma once

#include <vector>

#include <QColor>
#include <QPointF>
#include <QLineF>
#include <QPixmap>
#include <QIcon>
#include <QPainter>

#include "../Nodeeditor/Datatypes/intensitymap.h"

class Stencil
{
public:
    Stencil(QString filename, int index);
    Stencil(QString filename, double brush, int index);
    Stencil(QString filename, double brush, QColor color, int index);
    ~Stencil();

    void setColor(QColor color);
    void setAlpha(double value);
    void setBrush(double brush);
    void draw(QPainter *painter, QPointF pos);

    int index();
    QString name();
    QIcon icon();

private:
    int _index;
    QString _filename;
    void _generate();
    double _brush = 8.00;
    QColor _color{0, 0, 0, 128};
    IntensityMap _stencil;
    QPixmap _pixmap;
    QIcon _icon;
};

class StencilList
{
public:
    static StencilList *getInstance();
    ~StencilList();
    Stencil *stencil(int index);
    int count();

private:
    StencilList();
    static bool _instance;
    static StencilList *_single;
    std::vector<Stencil> _stencils;
};
