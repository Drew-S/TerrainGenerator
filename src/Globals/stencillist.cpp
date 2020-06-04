#include "stencillist.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QRegExp>
#include <QStringList>

#include <glm/vec4.hpp>

#include "../Nodeeditor/Datatypes/intensitymap.h"
#include "../Nodeeditor/Datatypes/vectormap.h"
#include "Globals/settings.h"

/******************************************************************************
 *                                STENCIL                                     *
 ******************************************************************************/

/**
 * Stencil
 * 
 * Create a new stencil from the provided file. Loads from texture. Sets the
 * index that the stencil is within the stencillist.
 * 
 * @param QString filename : The file to load the stencil texture from.
 * @param int index : The index that the stencil is in the stencillist.
 */
Stencil::Stencil(QString filename, int index)
{
    QPixmap pixmap(filename);
    this->_filename = filename;
    this->_stencil = IntensityMap(pixmap, IntensityMap::RED);

    // Generate icon of the stencil
    this->_icon = QIcon(VectorMap::fromIntensityMap(this->_stencil,
                                                    glm::dvec4(1.00,
                                                               1.00,
                                                               1.00,
                                                               1.00),
                                                    VectorMap::OVERRIDE_COLOR)
                            .toPixmap());
    this->_generate();
    this->_index = index;
    qDebug("Stencil '%s' (%d) loaded", qPrintable(this->name()), index);
}

/**
 * Stencil
 * 
 * Create a new stencil from the provided file. Loads from texture. Sets the
 * index that the stencil is within the stencillist. Sets the radius of the
 * stencil.
 * 
 * @param QString filename : The file to load the stencil texture from.
 * @param double brush : The diameter of the stencil.
 * @param int index : The index that the stencil exists in the stencillist.
 */
Stencil::Stencil(QString filename, double brush, int index)
{
    QPixmap pixmap(filename);
    this->_filename = filename;
    this->_stencil = IntensityMap(pixmap, IntensityMap::RED);
    this->_brush = brush;
    this->_icon = QIcon(VectorMap::fromIntensityMap(this->_stencil,
                                                    glm::dvec4(1.00, 1.00, 1.00, 1.00),
                                                    VectorMap::OVERRIDE_COLOR)
                            .toPixmap());
    this->_generate();
    this->_index = index;
}

/**
 * Stencil
 * 
 * Create a new stencil from the provided file. Loads from texture. Sets the
 * index that the stencil is within the stencillist. Sets the radius of the
 * stencil. Sets the colour of the stencil that should be used.
 * 
 * @param QString filename : The file to load the stencil texture from.
 * @param double brush : The stencil diameter.
 * @param QColor color : The stencil colour.
 * @param int index : The index that the stencil exists in the stencillist.
 */
Stencil::Stencil(QString filename, double brush, QColor color, int index)
{
    QPixmap pixmap(filename);
    this->_filename = filename;
    this->_stencil = IntensityMap(pixmap, IntensityMap::RED);
    this->_brush = brush;
    this->_color = color;
    this->_icon = QIcon(VectorMap::fromIntensityMap(this->_stencil,
                                                    glm::dvec4(1.00,
                                                               1.00,
                                                               1.00,
                                                               1.00),
                                                    VectorMap::OVERRIDE_COLOR)
                            .toPixmap());
    this->_generate();
    this->_index = index;
}

/**
 * setColor
 * 
 * Updates the stencils drawing colour.
 * 
 * @param QColor color : The new colour to be used by the stencil.
 */
void Stencil::setColor(QColor color)
{
    this->_color.setRed(color.red());
    this->_color.setGreen(color.green());
    this->_color.setBlue(color.blue());
    this->_generate();
}

/**
 * setBrush
 * 
 * Sets the stencil drawing diameter.
 * 
 * @param double brush : The diameter to be set.
 */
void Stencil::setBrush(double brush)
{
    this->_brush = brush;
    this->_generate();
}

/**
 * setAlpha
 * 
 * Set the opacity of the drawing stencil.
 * 
 * @param double value : The alpha value 
 */
void Stencil::setAlpha(double value)
{
    Q_ASSERT(0 <= value && value <= 1);
    this->_color.setAlphaF(value);
    this->_generate();
}

/**
 * draw
 * 
 * Draws the stencil with the provided painter (draw on pixmap with painter) at
 * a specific position.
 * 
 * @param QPainter* painter : The painter that is to be used to draw with.
 * @param QPointF pos : The position to draw the stencil to.
 */
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

/**
 * index
 * 
 * Return the index that this stencil is in the stencil list.
 * 
 * @returns int : The index that the stencil is in the stencil list.
 */
int Stencil::index()
{
    return this->_index;
}

/**
 * name
 * 
 * The name of the stencil. "my-stencil-brush.png" -> "my stencil brush".
 * 
 * @returns QString : The name of the stencil as displayed in the UI.
 */
QString Stencil::name()
{
    return QFileInfo(this->_filename)
        .fileName()
        .replace(QRegExp("\\.(jpg|png|jpeg)$", Qt::CaseInsensitive), "")
        .replace(QRegExp("-"), " ");
}

/**
 * icon
 * 
 * Returns the icon of the stencil, used for brush selection.
 * 
 * @returns QIcon : The icon of the stencil.
 */
QIcon Stencil::icon()
{
    return this->_icon;
}

/**
 * _generate
 * 
 * Generates the stencil and saves it as a pixmap (takes the texture and
 * converts the red channel into a mask for alpha that applies colour stamp).
 */
void Stencil::_generate()
{
    glm::dvec4 color(this->_color.redF(),
                     this->_color.greenF(),
                     this->_color.blueF(),
                     this->_color.alphaF());

    this->_pixmap = VectorMap::fromIntensityMap(
        this->_stencil,
        color,
        VectorMap::MASK_ALPHA)
    .toPixmap()
    .scaled(this->_brush, this->_brush, Qt::KeepAspectRatio);
}

/******************************************************************************
 *                              STENCILLIST                                   *
 ******************************************************************************/

// StencilList singleton setup
bool StencilList::_instance = false;
StencilList *StencilList::_single = nullptr;

/**
 * StencilList
 * 
 * Creates a stencil list singleton that loads all of the stencils into the
 * program for drawing with.
 */
StencilList::StencilList()
{
    // Get a list of stencils and load them into the program
    Q_CHECK_PTR(SETTINGS);
    std::vector<QDir> asset_dirs = SETTINGS->getAssetDirectories();
    qDebug("Initializing stencil brushes");
    for (int i = 0; i < (int)asset_dirs.size(); i++)
    {
        QDir stencils(QDir::cleanPath(asset_dirs[i].path() + QString("/stencils")));
        QStringList images = stencils.entryList(QStringList() << "*.jpg"
                                                              << "*.png"
                                                              << "*.jpeg",
                                                QDir::Files);
        for (int i = 0; i < images.size(); i++)
        {
            this->_stencils.push_back(Stencil(QString("assets/stencils/") + images.at(i), i));
        }
    }
}

/**
 * ~StencilList
 * 
 * Deletes the singleton from the application and marks its references as such.
 */
StencilList::~StencilList()
{
    _instance = false;
}

/**
 * stencil
 * 
 * Return a pointer of a specific stencil by index in the list, returns a
 * nullptr if the index is beyond the bounds of the list.
 * 
 * @param int index : The index of the stencil to obtain.
 * 
 * @returns Stencil* : The pointer to the requested stencil.
 */
Stencil *StencilList::stencil(int index)
{
    if (index < 0 || index >= (int)this->count())
        return nullptr;

    return &this->_stencils[index];
}

/**
 * count
 * 
 * Returns the number of stencils loaded into the list.
 * 
 * @returns int : The number of stencils in the list.
 */
int StencilList::count()
{
    return (int)this->_stencils.size();
}

/**
 * getInstance
 * 
 * Get the singleton instance of the StencilList, if the singleton has not been
 * created yet this creates the singleton.
 * 
 * @returns StencilList* : The pointer to the stencillist singleton.
 */
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
