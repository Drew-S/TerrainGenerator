#include "stencil.h"

#include "../../Datatypes/vectormap.h"

#include <glm/vec4.hpp>

#include <QRegExp>
#include <QFileInfo>
#include <QDebug>

Stencil::Stencil(QString filename, int index)
{
    QPixmap pixmap(filename);
    this->_filename = filename;
    this->_stencil = IntensityMap(pixmap, IntensityMap::AVERAGE);
    this->_icon = QIcon(VectorMap::fromIntensityMap(this->_stencil,
                                                    glm::dvec4(1.00, 1.00, 1.00, 1.00),
                                                    VectorMap::OVERRIDE_COLOR)
                            .toPixmap());
    this->_generate();
    this->_index = index;
    qDebug("Stencil '%s' (%d) loaded", qPrintable(this->name()), index);
}

Stencil::Stencil(QString filename, double brush, int index)
{
    QPixmap pixmap(filename);
    this->_filename = filename;
    this->_stencil = IntensityMap(pixmap, IntensityMap::AVERAGE);
    this->_brush = brush;
    this->_icon = QIcon(VectorMap::fromIntensityMap(this->_stencil,
                                                    glm::dvec4(1.00, 1.00, 1.00, 1.00),
                                                    VectorMap::OVERRIDE_COLOR)
                            .toPixmap());
    this->_generate();
    this->_index = index;
}

Stencil::Stencil(QString filename, double brush, QColor color, int index)
{
    QPixmap pixmap(filename);
    this->_filename = filename;
    this->_stencil = IntensityMap(pixmap, IntensityMap::AVERAGE);
    this->_brush = brush;
    this->_color = color;
    this->_icon = QIcon(VectorMap::fromIntensityMap(this->_stencil,
                                                    glm::dvec4(1.00, 1.00, 1.00, 1.00),
                                                    VectorMap::OVERRIDE_COLOR)
                            .toPixmap());
    this->_generate();
    this->_index = index;
}

Stencil::~Stencil() {}

void Stencil::setColor(QColor color)
{
    this->_color = color;
    this->_generate();
}

void Stencil::setBrush(double brush)
{
    this->_brush = brush;
    this->_generate();
}

void Stencil::draw(QPainter *painter, QPointF pos)
{
    painter->drawPixmap(
        pos.x() - (this->_brush / 2.00),
        pos.y() - (this->_brush / 2.00),
        this->_pixmap,
        0, 0,
        this->_brush, this->_brush);
}

int Stencil::index()
{
    return this->_index;
}

QString Stencil::name()
{
    return QFileInfo(this->_filename)
        .fileName()
        .replace(QRegExp("^brush(-*)", Qt::CaseInsensitive), "")
        .replace(QRegExp("\\.(jpg|png|jpeg)$", Qt::CaseInsensitive), "")
        .replace(QRegExp("-"), " ");
}

QIcon Stencil::icon()
{
    return this->_icon;
}

void Stencil::_generate()
{
    glm::dvec4 color(this->_color.redF(), this->_color.greenF(), this->_color.blueF(), this->_color.alphaF());
    this->_pixmap = VectorMap::fromIntensityMap(this->_stencil, color, VectorMap::MASK_ALPHA)
                        .toPixmap()
                        .scaled(this->_brush, this->_brush, Qt::KeepAspectRatio);
}