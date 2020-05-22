#include "vectormap.h"

#include <QDebug>

VectorMap::VectorMap() {}

// Set initial size without data
VectorMap::VectorMap(int width, int height)
{
    Q_ASSERT(width > 0);
    Q_ASSERT(height > 0);
    qDebug("Creating Vector Map with only size set: (%dx%d)", width, height);
    this->width = width;
    this->height = height;
}

// Set initial size with a specificied fill color
VectorMap::VectorMap(int width, int height, glm::dvec4 fill)
{
    Q_ASSERT(width > 0);
    Q_ASSERT(height > 0);
    qDebug("Creating Vector Map with only size set: (%dx%d)", width, height);
    this->_fill = fill;
    this->_use_fill = true;
    this->width = width;
    this->height = height;
}

// Set size and values from a 1D vector array
VectorMap::VectorMap(int width, int height, std::vector<glm::dvec4> values)
{
    Q_ASSERT(width > 0);
    Q_ASSERT(height > 0);
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
IntensityMap VectorMap::toIntensityMap(IntensityMap::Channel channel)
{
    qDebug("Converting Vector Map to Intensity Map");
    IntensityMap map(this->width, this->height);
    double c, min, max;
    glm::dvec4 val;
    for (int y = 0; y < this->height; y++)
    {
        for (int x = 0; x < this->width; x++)
        {
            switch (channel)
            {
            case IntensityMap::RED:
                map.append(this->at(x, y).r);
                break;

            case IntensityMap::GREEN:
                map.append(this->at(x, y).g);
                break;

            case IntensityMap::BLUE:
                map.append(this->at(x, y).b);
                break;

            case IntensityMap::ALPHA:
                map.append(this->at(x, y).a);
                break;

            case IntensityMap::AVERAGE:
                val = this->at(x, y);
                map.append((val.x + val.y + val.z + val.w) / 4.00);
                break;

            case IntensityMap::MIN:
                val = this->at(x, y);
                min = val.x;
                c = val.y;
                if (c < min)
                    min = c;
                c = val.z;
                if (c < min)
                    min = c;
                c = val.w;
                if (c < min)
                    min = c;
                map.append(min);
                break;

            case IntensityMap::MAX:
                val = this->at(x, y);
                max = val.x;
                c = val.y;
                if (c > max)
                    max = c;
                c = val.z;
                if (c > max)
                    max = c;
                c = val.w;
                if (c > max)
                    max = c;
                map.append(max);
                break;

            default:
                Q_UNREACHABLE();
                break;
            }
        }
    }

    qDebug("Completed conversion");

    return map;
}

// Create vector map from an intensity map
VectorMap VectorMap::fromIntensityMap(IntensityMap &map, glm::dvec4 color, VectorMap::ColorMode mode)
{
    qDebug("Converting Intensity Map to Vector Map");
    VectorMap vec(map.width, map.height);
    for (int y = 0; y < map.height; y++)
    {
        for (int x = 0; x < map.width; x++)
        {
            double v = map.at(x, y);
            switch (mode)
            {
            case VectorMap::APPLY:
                vec.append(color * v);
                break;
            case VectorMap::OVERRIDE_COLOR:
                vec.append(glm::dvec4(color.x * v, color.y * v, color.z * v, color.a));
                break;
            case VectorMap::OVERRIDE_MAP:
                vec.append(glm::dvec4(color.x * v, color.y * v, color.z * v, v));
                break;
            case VectorMap::MASK:
                vec.append(glm::dvec4(color.x, color.y, color.z, v));
                break;
            case VectorMap::MASK_ALPHA:
                vec.append(glm::dvec4(color.x, color.y, color.z, v * color.a));
                break;
            default:
                Q_UNREACHABLE();
                break;
            }
        }
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
            glm::dvec4 pixel = this->at(x, y);
            QColor color = QColor::fromRgbF(pixel.r, pixel.g, pixel.b, pixel.a);
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

VectorMap VectorMap::scaled(int width, int height)
{
    QImage image = this->toImage().scaled(width, height);
    return VectorMap(image);
}

// Check if the map is using a solid fill color
bool VectorMap::usingFill()
{
    return this->_use_fill;
}

// Transform a vector map using a provided lambda
VectorMap VectorMap::transform(glm::dvec4 func(glm::dvec4, glm::dvec4), glm::dvec4 value)
{
    VectorMap map;
    if (this->usingFill())
    {
        map = VectorMap(this->width, this->height, func(this->_fill, value));
    }
    else
    {
        map = VectorMap(this->width, this->height);
        for (int y = 0; y < this->height; y++)
            for (int x = 0; x < this->width; x++)
                map.append(func(this->at(x, y), value));
    }

    return map;
}
VectorMap VectorMap::transform(glm::dvec4 func(glm::dvec4, glm::dvec4), VectorMap *map)
{
    Q_CHECK_PTR(map);
    VectorMap out;
    if (this->usingFill() && map->usingFill())
    {
        out = VectorMap(this->width, this->height, func(this->_fill, map->at(0, 0)));
    }
    else
    {
        out = VectorMap(this->width, this->height);
        for (int y = 0; y < this->height; y++)
            for (int x = 0; x < this->width; x++)
                out.append(func(this->at(x, y), map->at(x, y)));
    }

    return out;
}

// Get a value at a specific index, returns the fill color if beyond bounds
// or empty pixels
glm::dvec4 VectorMap::at(int x, int y)
{
    if (x < 0 || x >= this->width || y < 0 || y >= this->height)
        return this->_fill;
    int index = y * this->width + x;
    if (index >= (int)this->values.size())
        return this->_fill;

    return this->values[y * this->width + x];
}

// Append a value to the array
bool VectorMap::append(glm::dvec4 value)
{
    if ((int)this->values.size() >= this->width * this->height)
        return false;
    this->_use_fill = false;
    this->values.push_back(value);
    return true;
}

// Set a specific value for an index
bool VectorMap::set(int x, int y, glm::dvec4 value)
{
    if (x < 0 || x >= this->width || y < 0 || y >= this->height)
        return false;
    int index = y * this->width + x;
    this->_use_fill = false;
    if (index >= (int)this->values.size())
        for (int i = (int)this->values.size(); i < index; i++)
            this->values.push_back(this->_fill);

    this->values[index] = value;
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
            this->values.push_back(glm::dvec4(color.redF(), color.greenF(), color.blueF(), color.alphaF()));
        }
    }
}
