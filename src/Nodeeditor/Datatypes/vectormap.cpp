#include "vectormap.h"

#include <QDebug>

/**
 * VectorMap
 * 
 * Create a new empty map.
 */
VectorMap::VectorMap() {}

/**
 * VectorMap
 *
 * Create a vector map with a set size and uses the default fill value.
 *
 * @param int width : The width of the map.
 * @param int height : The height of the map.
 */
VectorMap::VectorMap(int width, int height)
{
    Q_ASSERT(width > 0);
    Q_ASSERT(height > 0);
    qDebug("Creating Vector Map with only size set: (%dx%d)", width, height);
    this->width = width;
    this->height = height;
}

/**
 * VectorMap
 *
 * Create a vector map with a set size and provided fill value.
 *
 * @param int width : The width of the map.
 * @param int height : The height of the map.
 * @param glm::dvec4 fill : The fill value of the map.
 */
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

/**
 * VectorMap
 *
 * Create a vector map with a set size and a list of pixel values.
 *
 * @param int width : The width of the map.
 * @param int height : The height of the map.
 * @param std::vector<glm::dvec4> values : The pixel values of the map.
 */
VectorMap::VectorMap(int width, int height, std::vector<glm::dvec4> values)
{
    Q_ASSERT(width > 0);
    Q_ASSERT(height > 0);
    qDebug("Creating Vector Map with data: (%dx%d), %d pixels set",
           width,
           height,
           width * height);

    this->width = width;
    this->height = height;
    this->values = values;
}

/**
 * VectorMap
 *
 * Create a vector map from a provided image.
 *
 * @param QImage image : The image to create the vector map from.
 */
VectorMap::VectorMap(QImage image)
{
    qDebug("Creating Vector Map from QImage");
    this->_saveImage(image);
}

/**
 * VectorMap
 *
 * Create a vector map from a supplied pixmap.
 *
 * @param QPixmap pixmap : The provided pixmap.
 */
VectorMap::VectorMap(QPixmap image)
{
    qDebug("Creating Vector Map from QPixmap");
    this->_saveImage(image.toImage());
}

/**
 * toIntensityMap
 *
 * Convert the vector map to an intensity map.
 *
 * @param IntensityMap::Channel channel : The channel to select from.
 *
 * @returns IntensityMap : The newly created intensity map.
 */
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

/**
 * fromIntensityMap
 *
 * Create a vector map from an intensity map, using the selected color to be
 * applied to the intensity map with the application colour mode.
 *
 * @param IntensityMap& map : The intensity map to create the vector map from.
 * @param glm::dvec4 color : The colour to be applied to the intensity map.
 * @param VectorMap::ColorMode : The colour mode to be used in applying the
 *                               colour to the intensity map.
 *                               APPLY (rgba * intensity),
 *                               OVERRIDE_COLOR (rgb * intensity, a),
 *                               OVERRIDE_MAP (rgb * intensity, intensity),
 *                               MASK (rgb, intensity),
 *                               MASK_ALPHA (rgb, a * intensity)
 *
 * @returns VectorMap : The created map from the supplied intensity map.                               
 */
VectorMap VectorMap::fromIntensityMap(IntensityMap &map,
                                      glm::dvec4 color,
                                      VectorMap::ColorMode mode)
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
                vec.append(glm::dvec4(color.x * v,
                           color.y * v,
                           color.z * v,
                           color.a));
                break;
            case VectorMap::OVERRIDE_MAP:
                vec.append(glm::dvec4(color.x * v,
                                      color.y * v,
                                      color.z * v,
                                      v));
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

/**
 * toImage
 *
 * Converts the vector map to an image.
 *
 * @param bool print_qimage : Debug value whether to print converting or not.
 *
 * @returns QImage : The converted image.
 */
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

/**
 * toPixmap
 *
 * Convert the vector map to a pixmap.
 *
 * @returns QPixmap : The converted pixmap.
 */
QPixmap VectorMap::toPixmap()
{
    qDebug("Converting Vector Map to QPixmap");
    return QPixmap::fromImage(this->toImage(false));
}
 
/**
 * scaled
 *
 * Scaled the vector map to the specified size. Uses QImage under the hood to
 * scale the vector map using Qt's linear interpolation.
 *
 * @param int width : The newly set width.
 * @param int height : The new set height.
 *
 * @returns VectorMap : The scaled vetor map.
 */
VectorMap VectorMap::scaled(int width, int height)
{
    QImage image = this->toImage().scaled(width, height);
    return VectorMap(image);
}

/**
 * usingFill
 *
 * Returns whether or not the vector map is entirely defiend by the fill value.
 *
 * @returns bool : Whether or not the fill value is used for the entire map.
 */
bool VectorMap::usingFill()
{
    return this->_use_fill;
}

/**
 * transform
 *
 * Transforms the vector map using the supplied function and a fixed value.
 *
 * @param [](glm::dvec4, glm::dvec4) -> glm::dvec4 {} : The transformation
 *                                                      function.
 * @param glm::dvec4 value : The fixed value to be used in the transformation
 *                           function.
 */
VectorMap VectorMap::transform(glm::dvec4 func(glm::dvec4, glm::dvec4),
                               glm::dvec4 value)
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

/**
 * transform
 *
 * Transforms the vector map with s upplied function and another vector map.
 *
 * @param [](glm::dvec4, glm::dvec4) -> glm::dvec4 {} : The transformation
 *                                                      function.
 * @param VectorMap* map : The other vector map to convert transforms this map
 *                         with the function pixel by pixel.
 */
VectorMap VectorMap::transform(glm::dvec4 func(glm::dvec4, glm::dvec4),
                               VectorMap *map)
{
    Q_CHECK_PTR(map);
    VectorMap out;
    if (this->usingFill() && map->usingFill())
    {
        out = VectorMap(this->width,
                        this->height,
                        func(this->_fill, map->at(0, 0)));
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

/**
 * at
 *
 * Get the specifc value at a supplied index. Returns the fill value if the
 * index is beyond the bounds.
 *
 * @param int x : The column.
 * @param int y : The row.
 *
 * @returns glm::dvec4 : The value of the pixel index.
 */
glm::dvec4 VectorMap::at(int x, int y)
{
    if (x < 0 || x >= this->width || y < 0 || y >= this->height)
        return this->_fill;
    int index = y * this->width + x;
    if (index >= (int)this->values.size())
        return this->_fill;

    return this->values[y * this->width + x];
}

/**
 * append
 *
 * Appends values to the end of the value list.
 *
 * @param glm::dvec4 value : The value to append to the end of the list.
 *
 * @returns bool : Whether appending was successful or not.
 */
bool VectorMap::append(glm::dvec4 value)
{
    if ((int)this->values.size() >= this->width * this->height)
        return false;
    this->_use_fill = false;
    this->values.push_back(value);
    return true;
}

/**
 * set
 *
 * Replace a value at a specified index with a specified value if the index is
 * within accepted range.
 * 
 * @param int x : The column.
 * @param int y : The row.
 * @param glm::dvec4 value : The value to replace with.
 * 
 * @returns bool : Whether or not the value was applied.
 */
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

/**
 * _saveImage
 * 
 * Helper function used to convert QImage (or QPixmap converted to QImage) into
 * a vector map, takes the rgba and converts the colour into a glm::dev4 for
 * each pixel.
 * 
 * @param QImage image : The image to convert to a vector map.
 */
void VectorMap::_saveImage(QImage image)
{
    this->height = image.height();
    this->width = image.width();
    for (int y = 0; y < this->height; y++)
    {
        for (int x = 0; x < this->width; x++)
        {
            QColor color = image.pixelColor(x, y);
            this->values.push_back(glm::dvec4(color.redF(),
                                              color.greenF(),
                                              color.blueF(),
                                              color.alphaF()));
        }
    }
}
