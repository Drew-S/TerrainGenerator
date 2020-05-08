#include "vectormap.h"

#include <QDebug>

VectorMap::VectorMap() {}

// Set initial size without data
VectorMap::VectorMap(int width, int height)
{
    qDebug("Creating Vector Map with only size set: (%dx%d)", width, height);
    this->width = width;
    this->height = height;
}

// Set size and values from a 1D vector array
VectorMap::VectorMap(int width, int height, std::vector<glm::dvec3> values)
{
    qDebug("Creating Vector Map with data: (%dx%d), %d pixels set", width, height, width * height);
    this->width = width;
    this->height = height;
    this->values = values;
}

// Create vector map from image
VectorMap::VectorMap(QImage image)
{
    qDebug("Creating Vector Map from QImage");
    this->_saveImage(image);
}

// Create vector map from pixmap
VectorMap::VectorMap(QPixmap image)
{
    qDebug("Creating Vector Map from QPixmap");
    this->_saveImage(image.toImage());
}

VectorMap::~VectorMap() {}

// Convert to an intensity map
// TODO: Add support for channel selecting
IntensityMap VectorMap::toIntensityMap()
{
    qDebug("Converting Vector Map to Intensity Map");
    IntensityMap map(this->width, this->height);
    for (int i = 0; i < this->width * this->height; i++)
        map.append(this->values[i].x);

    return map;
}

// Create vector map from an intensity map
VectorMap VectorMap::fromIntensityMap(IntensityMap &map)
{
    qDebug("Converting Intensity Map to Vector Map");
    VectorMap vec(map.width, map.height);
    for (int i = 0; i < map.width * map.height; i++)
    {
        double v = map.values[i];
        vec.append(glm::dvec3(v, v, v));
    }
    return vec;
}

// Convert vector map to an image
QImage VectorMap::toImage(bool print_qimage)
{
    if (print_qimage)
        qDebug("Converting Vector Map to QImage");
    QImage image(this->width, this->height, QImage::Format_RGBA64);
    for (int y = 0; y < this->height; y++)
    {
        for (int x = 0; x < this->width; x++)
        {
            glm::dvec3 pixel = this->at(x, y);
            QColor color = QColor::fromRgbF(pixel.x, pixel.y, pixel.z);
            image.setPixelColor(x, y, color);
        }
    }
    return image;
}

// Convert vector map to a pixmap
QPixmap VectorMap::toPixmap()
{
    qDebug("Converting Vector Map to QPixmap");
    return QPixmap::fromImage(this->toImage(false));
}

// Get a value at a specific index
glm::dvec3 VectorMap::at(int x, int y)
{
    if (x < 0 || x >= this->width || y < 0 || y >= this->height)
        return glm::dvec3(0.00, 0.00, 0.00);
    return this->values[y * this->width + x];
}

// Append a value to the array
bool VectorMap::append(glm::dvec3 value)
{
    if ((int)this->values.size() >= this->width * this->height)
        return false;
    this->values.push_back(value);
    return true;
}

// Set a specific value for an index
bool VectorMap::set(int x, int y, glm::dvec3 value)
{
    if (x < 0 || x >= this->width || y < 0 || y >= this->height)
        return false;
    this->values[y * this->width + x] = value;
    return true;
}

// Converts an image to a vector map
void VectorMap::_saveImage(QImage image)
{
    this->width = image.width();
    this->height = image.height();
    for (int y = 0; y < this->height; y++)
    {
        for (int x = 0; x < this->width; x++)
        {
            QColor color = image.pixelColor(x, y);
            this->values.push_back(glm::dvec3(color.redF(), color.greenF(), color.blueF()));
        }
    }
}
