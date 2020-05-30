#include "texturelist.h"

#include <QBrush>
#include <QColor>
#include <QRect>
#include <QFileInfo>
#include <QBuffer>
#include <QDebug>

// Constructors
Texture::Texture() {}
Texture::Texture(QString filename)
{
    this->_filename = filename;
    this->_pixmap = QPixmap(filename);
    this->_painter = new QPainter(&this->_pixmap);
}
Texture::Texture(QByteArray data, QString filename)
{
    this->_pixmap.loadFromData(data, QFileInfo(filename).suffix().toStdString().c_str());
    this->_filename = filename;
    this->_painter = new QPainter(&this->_pixmap);
}
Texture::Texture(int w, int h)
{
    QImage image(w, h, QImage::Format_ARGB32);
    this->_generated = true;
    image.fill(QColor(255, 255, 255, 255));
    this->_pixmap = QPixmap::fromImage(image);
    this->_painter = new QPainter(&this->_pixmap);
}
Texture::Texture(int w, int h, QString filename)
{
    this->_filename = filename;
    this->_generated = true;
    QImage image(w, h, QImage::Format_ARGB32);
    image.fill(QColor(255, 255, 255, 255));
    this->_pixmap = QPixmap::fromImage(image);
    this->_painter = new QPainter(&this->_pixmap);
}
Texture::~Texture() {}

// Return a pixmap
QPixmap Texture::pixmap(int scale)
{
    return scale <= 0 ? this->_pixmap : this->_pixmap.scaled(scale, scale);
}

// Return an image
QImage Texture::image(int scale)
{
    return this->pixmap(scale).toImage();
}

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

bool Texture::generated()
{
    return this->_generated;
}
bool Texture::edited()
{
    return this->_edited;
}

// Return an icon
QIcon Texture::icon()
{
    return QIcon(this->_pixmap);
}

// Return a vector map
VectorMap Texture::vectorMap(int scale)
{
    return VectorMap(this->pixmap(scale));
}

// Replace the underlying pixmap
void Texture::replace(QPixmap pixmap)
{
    qDebug("Updated texture");
    this->_pixmap = QPixmap(pixmap);
    this->_edited = true;
    emit this->updated();
}

// Return an intensity map
IntensityMap Texture::intensityMap(IntensityMap::Channel channel, int scale)
{
    return IntensityMap(this->pixmap(scale), channel);
}

QString Texture::name()
{
    return QFileInfo(this->_filename).fileName();
}

QString Texture::filename()
{
    return this->_filename;
}

QString Texture::saveName()
{
    QString name = this->name();
    if (this->_generated)
        return name.replace(QRegExp("\\.png$"), "") + QString(".png");

    return name;
}

// Draw the stencil onto the pixmap
void Texture::draw(Stencil *stencil, QPointF pos)
{
    Q_CHECK_PTR(stencil);
    Q_CHECK_PTR(this->_painter);
    stencil->draw(this->_painter, pos);
    emit this->updated();
}

// Save to the existing file
bool Texture::save()
{
    if (this->_filename == "")
        return false;
    return this->saveAs(this->_filename);
}

bool Texture::save(QIODevice *device)
{
    if (this->_generated)
        return this->_pixmap.save(device, "PNG");

    QFileInfo info(this->_filename);

    return this->_pixmap.save(device, info.suffix().toStdString().c_str());
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
    Q_CHECK_PTR(_single);
    return _single;
}

// Add a texture with the given file, returns the index for at() use
// TODO: Add file exist checks
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
int TextureList::loadTexture(QByteArray data, QString filename)
{
    qDebug() << "Loading Texture" << filename;
    Texture *t = new Texture(data, filename);
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
    return (int)this->_textures.size();
}

// Get the refernce for a specific texture,
// returns nullptr if the index beyond bounds
Texture *TextureList::at(int index)
{
    if (index < 0 || index >= (int)this->_textures.size())
        return nullptr;

    Q_CHECK_PTR(this->_textures[index]);
    return this->_textures[index];
}

// Find a loaded texture using its filename
int TextureList::find(QString filename)
{
    for (int i = 0; i < (int)this->_textures.size(); i++)
        if (this->_textures[i]->name() == filename)
            return i;

    return -1;
}