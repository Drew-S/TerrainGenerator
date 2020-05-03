#include "vectormap.h"

VectorMap::VectorMap() {}

// Set initial size without data
VectorMap::VectorMap(int width, int height)
{
    this->width = width;
    this->height = height;
}

// Set size and values from a 1D vector array
VectorMap::VectorMap(int width, int height, std::vector<glm::dvec3> values)
{
    this->width = width;
    this->height = height;
    this->values = values;
}

// Create vector map from image
VectorMap::VectorMap(QImage image)
{
    this->_saveImage(image);
}

// Create vector map from pixmap
VectorMap::VectorMap(QPixmap image)
{
    this->_saveImage(image.toImage());
}

VectorMap::~VectorMap() {}

// Convert to an intensity map
// TODO: Add support for channel selecting
IntensityMap VectorMap::toIntensityMap()
{
    IntensityMap map(this->width, this->height);
    for (int x = 0; x < this->width; x++)
    {
        for (int y = 0; y < this->height; y++)
        {
            map.append(this->at(x, y).x);
        }
    }
    return map;
}

// Create vector map from an intensity map
VectorMap VectorMap::fromIntensityMap(IntensityMap &map)
{

    VectorMap vec(map.width, map.height);
    for (int x = 0; x < map.width; x++)
    {
        for (int y = 0; y < map.height; y++)
        {
            double v = map.at(x, y);
            vec.append(glm::dvec3(v, v, v));
        }
    }
    return vec;
}

// Convert vector map to an image
QImage VectorMap::toImage()
{
    QImage image(this->width, this->height, QImage::Format_RGBA64);
    for (int x = 0; x < this->width; x++)
    {
        for (int y = 0; y < this->height; y++)
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
    return QPixmap::fromImage(this->toImage());
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
    if (this->values.size() >= this->width * this->height)
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
    for (int x = 0; x < this->width; x++)
    {
        for (int y = 0; y < this->height; y++)
        {
            QColor color = image.pixelColor(x, y);
            this->values.push_back(glm::dvec3(color.redF(), color.greenF(), color.blueF()));
        }
    }
}