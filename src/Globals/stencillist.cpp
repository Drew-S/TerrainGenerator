#include "stencillist.h"

#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QRegExp>
#include <QFileInfo>

#include "../Nodeeditor/Datatypes/vectormap.h"

#include <glm/vec4.hpp>

// Create a new stencil
Stencil::Stencil(QString filename, int index)
{
    QPixmap pixmap(filename);
    this->_filename = filename;
    this->_stencil = IntensityMap(pixmap, IntensityMap::AVERAGE);

    // Generate icon of the stencil
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

// Set the stencil color, ignores the alpha
void Stencil::setColor(QColor color)
{
    this->_color.setRed(color.red());
    this->_color.setGreen(color.green());
    this->_color.setBlue(color.blue());
    this->_generate();
}

// Set the brush radius
void Stencil::setBrush(double brush)
{
    this->_brush = brush;
    this->_generate();
}

// Set the stencils opacity
void Stencil::setAlpha(double value)
{
    this->_color.setAlphaF(value);
}

// Draw onto the given painter with the position
void Stencil::draw(QPainter *painter, QPointF pos)
{
    Q_CHECK_PTR(painter);
    painter->drawPixmap(
        pos.x() - (this->_brush / 2.00),
        pos.y() - (this->_brush / 2.00),
        this->_pixmap,
        0, 0,
        this->_brush, this->_brush);
}

// Get the index of the stencil in the stencil list
int Stencil::index()
{
    return this->_index;
}

// Get the name of the brush
QString Stencil::name()
{
    return QFileInfo(this->_filename)
        .fileName()
        .replace(QRegExp("\\.(jpg|png|jpeg)$", Qt::CaseInsensitive), "")
        .replace(QRegExp("-"), " ");
}

// Get the icon representation of the stencil
QIcon Stencil::icon()
{
    return this->_icon;
}

// Generate the drawing pixmap used to stamp with the QPainter
void Stencil::_generate()
{
    glm::dvec4 color(this->_color.redF(), this->_color.greenF(), this->_color.blueF(), this->_color.alphaF());
    this->_pixmap = VectorMap::fromIntensityMap(this->_stencil, color, VectorMap::MASK_ALPHA)
                        .toPixmap()
                        .scaled(this->_brush, this->_brush, Qt::KeepAspectRatio);
}

// StencilList singleton setup
bool StencilList::_instance = false;
StencilList *StencilList::_single = nullptr;

StencilList::StencilList()
{
    // Get a list of stencils and load them into the program
    // TODO: Use absolute path system
    QDir stencils("assets/stencils");
    QStringList images = stencils.entryList(QStringList() << "*.jpg"
                                                          << "*.png"
                                                          << "*.jpeg",
                                            QDir::Files);

    qDebug("Initializing stencil brushes");
    for (int i = 0; i < images.size(); i++)
    {
        this->_stencils.push_back(Stencil(QString("assets/stencils/") + images.at(i), i));
    }
}
StencilList::~StencilList()
{
    _instance = false;
}

// Get a reference to a specific stencil
Stencil *StencilList::stencil(int index)
{
    if (index < 0 || index >= this->count())
        return nullptr;

    return &this->_stencils[index];
}

// Get the number of stencils in the program
int StencilList::count()
{
    return (int)this->_stencils.size();
}

// Get/create a reference to the StencilList singleton
StencilList *StencilList::getInstance()
{
    if (!_instance)
    {
        _instance = true;
        _single = new StencilList();
    }
    Q_CHECK_PTR(_single);
    return _single;
}
