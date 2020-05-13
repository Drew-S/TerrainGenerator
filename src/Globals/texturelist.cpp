#include "texturelist.h"

#include <QBrush>
#include <QColor>
#include <QRect>
#include <QFileInfo>

// Constructors
Texture::Texture() {}
Texture::Texture(QString filename)
{
    this->_filename = filename;
    this->_pixmap = QPixmap(filename);
    this->_painter = new QPainter(&this->_pixmap);
}
Texture::Texture(int w, int h)
{
    this->_pixmap = QPixmap(w, h);
    this->_painter = new QPainter(&this->_pixmap);
    this->_painter->setBrush(QBrush(QColor(255, 255, 255, 255)));
    this->_painter->drawRect(QRect(0, 0, w, h));
}
Texture::Texture(int w, int h, QString filename)
{
    this->_filename = filename;
    this->_pixmap = QPixmap(w, h);
    this->_painter = new QPainter(&this->_pixmap);
    this->_painter->setBrush(QBrush(QColor(255, 255, 255, 255)));
    this->_painter->drawRect(QRect(0, 0, w, h));
}
Texture::~Texture() {}

// Return a pixmap
QPixmap Texture::pixmap() { return this->_pixmap; }

// Return an image
QImage Texture::image() { return this->_pixmap.toImage(); }

// Return an icon
QIcon Texture::icon() { return QIcon(this->_pixmap); }

// Return a vector map
VectorMap Texture::vectorMap() { return VectorMap(this->_pixmap); }

// Replace the underlying pixmap
void Texture::replace(QPixmap pixmap) { this->_pixmap = pixmap; }

// Return an intensity map
IntensityMap Texture::intensityMap(IntensityMap::Channel channel) { return IntensityMap(this->_pixmap, channel); }

QString Texture::name()
{
    return QFileInfo(this->_filename).fileName();
}

// Draw the stencil onto the pixmap
void Texture::draw(Stencil *stencil, QPointF pos, bool update)
{
    stencil->draw(this->_painter, pos);
    if (update)
        emit this->updated();
}

// Emit updated signal
void Texture::apply()
{
    emit this->updated();
}

// Save to the existing file
bool Texture::save()
{
    if (this->_filename == "")
        return false;
    return this->saveAs(this->_filename);
}

// Save to a specific file
bool Texture::saveAs(QString filename)
{
    return this->_pixmap.save(filename, 0, 100);
}

// Singleton setup
bool TextureList::_instance = false;
TextureList *TextureList::_single = nullptr;
int TextureList::_num = 1;

TextureList::TextureList() {}

TextureList::~TextureList()
{
    _instance = false;
}

// Get/create refernce to the singleton
TextureList *TextureList::getInstance()
{
    if (!_instance)
    {
        _instance = true;
        _single = new TextureList();
    }
    return _single;
}

// Add a texture with the given file, returns the index for at() use
// TODO: Add duplicate file check
// TODO: Add file exists check
int TextureList::addTexture(QString filename)
{
    Texture *t = new Texture(filename);
    this->_textures.push_back(t);
    emit this->textureAdded((int)this->_textures.size() - 1);
    return (int)this->_textures.size() - 1;
}

// Add a texture with the given size, returns the index for at() use
int TextureList::addTexture(int w, int h)
{
    Texture *t = new Texture(w, h, QString("Texture_") + QString::number(_num));
    _num++;
    this->_textures.push_back(t);
    emit this->textureAdded((int)this->_textures.size() - 1);
    return (int)this->_textures.size() - 1;
}

// Get the number of textures listed
int TextureList::count()
{
    return (int)this->_textures.size() - 1;
}

// Get the refernce for a specific texture,
// returns nullptr if the index beyond bounds
Texture *TextureList::at(int index)
{
    if (index < 0 || index >= (int)this->_textures.size())
        return nullptr;
    return this->_textures[index];
}