#include "texturelist.h"

#include <QBrush>
#include <QBuffer>
#include <QColor>
#include <QDebug>
#include <QFileInfo>
#include <QRect>

/******************************************************************************
 *                                 TEXTURE                                    *
 ******************************************************************************/

/**
 * Texture
 * 
 * Plain constructor for complete new texture. (not used, may remove)
 */
Texture::Texture() {}

/**
 * Texture
 * 
 * Constructs a texture from the given filename.
 * 
 * @param QString filename : The file to create the texture from.
 */
Texture::Texture(QString filename)
{
    this->_filename = filename;
    this->_pixmap = QPixmap(filename);
    this->_painter = new QPainter(&this->_pixmap);
}

/**
 * Texture
 * 
 * Create a texture from a raw byte array, used for loading the texture directly
 * from the save zip file that is loaded with Quazip.
 * 
 * @param QByteArray data : The raw data of the texture.
 * @param QString filename : The filename of the texture.
 */
Texture::Texture(QByteArray data, QString filename)
{
    this->_pixmap.loadFromData(data, QFileInfo(filename).suffix().toStdString().c_str());
    this->_filename = filename;
    this->_painter = new QPainter(&this->_pixmap);
}

/**
 * Texture
 * 
 * Creates a new empty texture (for drawing on) with only the width and height
 * set.
 * 
 * @param int w : The width of the new texture.
 * @param int h : The height of the new texture.
 */
Texture::Texture(int w, int h)
{
    QImage image(w, h, QImage::Format_ARGB32);
    this->_generated = true;
    image.fill(QColor(255, 255, 255, 255));
    this->_pixmap = QPixmap::fromImage(image);
    this->_painter = new QPainter(&this->_pixmap);
}

/**
 * Texture
 * 
 * Creates a new empty texture (for drawing on) with the width, height, and
 * filename set.
 * 
 * @param int w : The width of the new texture.
 * @param int h : The height of the new texture.
 * @param QString filename : The filname of the texture (Texture_#).
 */
Texture::Texture(int w, int h, QString filename)
{
    this->_filename = filename;
    this->_generated = true;
    QImage image(w, h, QImage::Format_ARGB32);
    image.fill(QColor(255, 255, 255, 255));
    this->_pixmap = QPixmap::fromImage(image);
    this->_painter = new QPainter(&this->_pixmap);
}

/**
 * pixmap
 * 
 * Returns the QPixmap of the texture file.
 * 
 * @param int scale : The scale that the pixmap should be done to, if the value
 *                    is less than or 0 then no scale is applied.
 * 
 * @returns QPixmap : The pixmap that is used.
 */
QPixmap Texture::pixmap(int scale)
{
    return scale <= 0 ? this->_pixmap : this->_pixmap.scaled(scale, scale);
}

/**
 * image
 * 
 * Returns the QImage of the texture file. Uses pixmap above for returning the
 * image.
 * 
 * @param int scale : The scale of the image.
 * 
 * @returns QImage : The image of the texture.
 */
QImage Texture::image(int scale)
{
    return this->pixmap(scale).toImage();
}

/**
 * bytes
 * 
 * Saves the texture to the provided byte array pointer. This saves the raw data
 * of the image to the byte array (used for saving to zip file). Saves generated
 * textures using the png format. Loaded files use the file format.
 * 
 * @param QByteArray* bytes : The pointer to the bytearray to save the data to.
 * 
 * @returns bool : Wether or not the operation was successful.
 */
bool Texture::bytes(QByteArray *bytes)
{
    Q_CHECK_PTR(bytes);
    QBuffer buffer(bytes);
    buffer.open(QIODevice::WriteOnly);

    qDebug() << this->_pixmap;
    if (this->generated() && this->_pixmap.save(&buffer, "PNG"))
    {
        buffer.close();
        return true;
    }
    else if (this->_pixmap.save(&buffer))
    {
        buffer.close();
        return true;
    }
    return false;
}

/**
 * generated
 * 
 * Returns whether or not the texture was generated or not.
 * 
 * @returns bool : Whether or not the texture is generated.
 */
bool Texture::generated()
{
    return this->_generated;
}

/**
 * edited
 * 
 * Returns whether or not the texture was edited or not.
 * 
 * @returns bool : Whether or not the texture is edited.
 */
bool Texture::edited()
{
    return this->_edited;
}

/**
 * icon
 * 
 * Returns the icon object of the texture.
 * 
 * @returns QIcon : The icon of the texture.
 */
QIcon Texture::icon()
{
    return QIcon(this->_pixmap);
}

/**
 * vectorMap
 * 
 * Returns a vector map, which uses higher detail values of the texture.
 * 
 * @param int scale : The scale of the output vector map.
 * 
 * @returns VectorMap : The vector map of the texture.
 */
VectorMap Texture::vectorMap(int scale)
{ 
    return VectorMap(this->pixmap(scale));
}

/**
 * replace
 * 
 * Used by the drawing system, this replaces the texture with the provided
 * pixmap, this replacement is the saved drawn version of the texture. Currently
 * there is no way to undo this process.
 * 
 * @param QPixmap pixmap : The new pixmap that replaces the internal one.
 * 
 * @signals updated
 * 
 * TODO: Add a history option to allow for this operation to be undone. This can
 *       be done by using temp files.
 */
void Texture::replace(QPixmap pixmap)
{
    qDebug("Updated texture");
    this->_pixmap = QPixmap(pixmap);
    this->_edited = true;
    emit this->updated();
}

/**
 * intensityMap
 * 
 * Returns the intensity map (a mono color vector map).
 * 
 * @param IntensityMap::Channel channel : The channel to use for combining the
 *                                        pixel colours into a mono channel.
 * @param int scale : The scale of the output intensity map.
 */
IntensityMap Texture::intensityMap(IntensityMap::Channel channel, int scale)
{
    return IntensityMap(this->pixmap(scale), channel);
}

/**
 * name
 * 
 * The name of the texture (filename). "/path/to/myfile.png" -> "myfile.png".
 * 
 * @returns QString : The name of the file.
 */
QString Texture::name()
{
    return QFileInfo(this->_filename).fileName();
}

/**
 * filename
 * 
 * The name of the texture (filename). "/path/to/myfile.png" ->
 * "/path/to/myfile.png"
 * 
 * @returns QString : The full filename of the file.
 */
QString Texture::filename()
{
    return this->_filename;
}
/**
 * saveName
 * 
 * The name of the texture (filename). "/path/to/myfile.png" -> "myfile.png"
 * "Texture_#" -> "Texture_#.png"
 * 
 * @returns QString : The save name of the file.
 */
QString Texture::saveName()
{
    QString name = this->name();
    if (this->_generated)
        return name.replace(QRegExp("\\.png$"), "") + QString(".png");

    return name;
}

/**
 * draw
 * 
 * Draw the stencil onto the stencil directly.
 * 
 * @param Stencil* stencil : The stencil to draw with.
 * @param QPointF pos : The position that the stencil should draw to.
 * 
 * @signals updated
 */
void Texture::draw(Stencil *stencil, QPointF pos)
{
    Q_CHECK_PTR(stencil);
    Q_CHECK_PTR(this->_painter);
    stencil->draw(this->_painter, pos);
    emit this->updated();
}

/**
 * save
 * 
 * Save the texture to an existing file.
 * 
 * @returns bool : Whether or not the save was successful.
 * 
 * TODO: Add option that if the texture is generated we first let the user
 *       select an output file.
 */
bool Texture::save()
{
    if (this->_filename == "")
        return false;
    return this->saveAs(this->_filename);
}

/**
 * save
 * 
 * Saves the file to the device provided (saves directly).
 * 
 * @param QIODevice* device : Pointer to the device to save to directly.
 */
bool Texture::save(QIODevice *device)
{
    if (this->_generated)
        return this->_pixmap.save(device, "PNG");

    QFileInfo info(this->_filename);

    return this->_pixmap.save(device, info.suffix().toStdString().c_str());
}

/**
 * saveAs
 * 
 * Saves the texture to a specific location on the file using the filename.
 * 
 * @param QString filename : The file to save to.
 * 
 * @returns bool : Whether or not the operation was successful.
 */
bool Texture::saveAs(QString filename)
{
    return this->_pixmap.save(filename, 0, 100);
}

/******************************************************************************
 *                               TEXTURELIST                                  *
 ******************************************************************************/

// Singleton setup
bool TextureList::_instance = false;
TextureList *TextureList::_single = nullptr;
int TextureList::_num = 1;

/**
 * TextureList
 * 
 * Creates the new texture list singleton.
 */
TextureList::TextureList() {}

/**
 * ~TextureList
 * 
 * Destroys the texture list singleton and sets the intstance reference.
 */
TextureList::~TextureList()
{
    _instance = false;
}

/**
 * getInstance
 * 
 * Returns a pointer to the texture list singleton. Creates the singleton if it
 * has not yet been created.
 * 
 * @returns TextureList* : The pointer to the singleton instance.
 */
TextureList *TextureList::getInstance()
{
    if (!_instance)
    {
        _instance = true;
        _single = new TextureList();
    }
    Q_CHECK_PTR(_single);
    return _single;
}

/**
 * addTexture
 * 
 * Adds a texture to the texturelist through the filename.
 * 
 * @param QString filename : The file that the texture should be created from.
 * 
 * @returns int : The index of the newly created texture.
 * 
 * @signals textureAdded
 * 
 * TODO: Add file exist checks.
 */
int TextureList::addTexture(QString filename)
{
    for (int i = 0; i < (int)this->_textures.size(); i++)
        if (this->_textures[i]->filename() == filename)
            return i;

    Texture *t = new Texture(filename);
    this->_textures.push_back(t);
    emit this->textureAdded((int)this->_textures.size() - 1);
    return (int)this->_textures.size() - 1;
}

/**
 * loadTexture
 * 
 * Creates a texture directly from the raw byte data and provided filename. Used
 * for loading from save files.
 * 
 * @param QByteArray data : The raw data of the file.
 * @param QString filename : The filename of the texture.
 * 
 * @returns int : The index of the newly created texture.
 * 
 * @signals textureAdded
 */
int TextureList::loadTexture(QByteArray data, QString filename)
{
    qDebug() << "Loading Texture" << filename;
    Texture *t = new Texture(data, filename);
    this->_textures.push_back(t);
    emit this->textureAdded((int)this->_textures.size() - 1);
    return (int)this->_textures.size() - 1;

}

/**
 * addTexture
 * 
 * Creates a new empty texture with the set width and height. When called from
 * user interaction width and height match as it is a square texture.
 * 
 * @param int w : The width of the new texture.
 * @param int h : The height of the new texture.
 * 
 * @returns int : The index of the new texture.
 * 
 * @signals textureAdded
 */
int TextureList::addTexture(int w, int h)
{
    Texture *t = new Texture(w, h, QString("Texture_") + QString::number(_num));
    _num++;
    this->_textures.push_back(t);
    emit this->textureAdded((int)this->_textures.size() - 1);
    return (int)this->_textures.size() - 1;
}

/**
 * count
 * 
 * Get the number of textures currently loaded into the system.
 * 
 * @returns int : The number of textures.
 */
int TextureList::count()
{
    return (int)this->_textures.size();
}

// Get the refernce for a specific texture,
// returns nullptr if the index beyond bounds
/**
 * at
 * 
 * Get a pointer to a texture at a specific index. Returns a nullptr if the
 * index is beyond the bounds of the list.
 * 
 * @param int index : The index of the texture to get.
 * 
 * @returns Texture* : The pointer to the texture.
 */
Texture *TextureList::at(int index)
{
    if (index < 0 || index >= (int)this->_textures.size())
        return nullptr;

    Q_CHECK_PTR(this->_textures[index]);
    return this->_textures[index];
}

/**
 * find
 * 
 * Finds the index of a texture by filename. Returns -1 if no texture is found.
 * 
 * @param QString filename : The name of the texture to find.
 * 
 * @returns int : The index of the texture.
 */
int TextureList::find(QString filename)
{
    for (int i = 0; i < (int)this->_textures.size(); i++)
        if (this->_textures[i]->name() == filename)
            return i;

    return -1;
}