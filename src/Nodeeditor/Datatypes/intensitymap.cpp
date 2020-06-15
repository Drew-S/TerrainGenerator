#include "intensitymap.h"

#include <math.h>

#include <QDebug>

/**
 * clamp
 * 
 * Clamps the value x between low and high, low <= x <= high
 * 
 * @param int x : The value to clamp.
 * @param int low : The low value to clamp to.
 * @param int high : The high value to clamp to.
 * 
 * @returns int : The clamped value.
 */
static int clamp(int x, int low, int high)
{
    return std::max(low, std::min(high, x));
}

/**
 * IntensityMap
 * 
 * Creates a new empty map.
 */
IntensityMap::IntensityMap() {}

/**
 * IntensityMap
 *
 * Create a new intensity map with a fixed size. The fill value inherits the
 * default value. Fill is used instead of an array of values if the entire map
 * is the same "colour."
 *
 * @param int width : The width of the map;
 * @param int height : The height of the map;
 */
IntensityMap::IntensityMap(int width, int height)
{
    Q_ASSERT(width > 0);
    Q_ASSERT(height > 0);
    qDebug("Creating Intensity Map with only size set: (%dx%d)", width, height);
    this->width = width;
    this->height = height;
}

/**
 * IntensityMap
 *
 * Create a new intensity map with a fixed size and a specific fill value.
 *
 * @param int width : The width of the map.
 * @param int height : The height of the map.
 * @param double fill : The fill value of the entire map.
 */
IntensityMap::IntensityMap(int width, int height, double fill)
{
    Q_ASSERT(width > 0);
    Q_ASSERT(height > 0);
    qDebug("Creating Intensity Map with only size set: (%dx%d)", width, height);
    this->width = width;
    this->height = height;
    this->_fill = fill;
    this->_use_fill = true;
}

/**
 * IntensityMap
 *
 * Create a new intensity map with a set size and with the mapped fill values
 * for each pixel being set.
 *
 * @param int width : The width of the map.
 * @param int height : The height of the map.
 * @param std::vector<double> values : Each individual pixel value.
 */
IntensityMap::IntensityMap(int width, int height, std::vector<double> values)
{
    Q_ASSERT(width > 0);
    Q_ASSERT(height > 0);
    qDebug("Creating Intensity Map with data: (%dx%d), %d pixels set",
           width,
           height,
           width * height);

    this->width = width;
    this->height = height;
    this->values = values;
}

/**
 * IntensityMap
 *
 * Create the intensity map from a supplied image. The channel selected
 * determines how to select the value for the intensity for each pixel.
 *
 * @param QImage image : The input image to create map from.
 * @param IntensityMap::Channel channel : The channel to select value.
 *                                        RED, GREEN, BLUE, ALPHA,
 *                                        AVERAGE ((r + b + g + a) / 4),
 *                                        MIN (smallest channel), MAX (largest)
 */
IntensityMap::IntensityMap(QImage image, IntensityMap::Channel channel)
{
    qDebug("Creating Intensity Map from QImage");
    this->_saveImage(image, channel);
}

/**
 * IntensityMap
 *
 * Create the intensity map from a supplied pixmap. See IntensityMap above for
 * image to see how this works the same.
 *
 * @param QPixmap pixmap : The input puxmap to create map from.
 * @param IntensityMap::Channel channel : The channel to select pixel value.
 */
IntensityMap::IntensityMap(QPixmap image, IntensityMap::Channel channel)
{
    qDebug("Creating Intensity Map from QPixmap");
    this->_saveImage(image.toImage(), channel);
}

/**
 * toImage
 *
 * Returns the intensity map as an image with the value copied across all
 * channels.
 *
 * @param bool print_qimage : Debugging option, the user should not use this.
 *
 * @returns QImage : The image.
 */
QImage IntensityMap::toImage(bool print_qimage)
{
    // Small debugging fix, since pixmap uses qimage to get data,
    // we overwrite printing with pixmap debug
    if (print_qimage)
        qDebug("Converting Intensity map to QImage");
    QImage image(this->width, this->height, QImage::Format_RGBA64);
    if (this->usingFill())
    {
        image.fill(QColor::fromRgbF(this->_fill,
                                    this->_fill,
                                    this->_fill,
                                    this->_fill));
    }
    else
    {
        for (int y = 0; y < this->height; y++)
        {
            for (int x = 0; x < this->width; x++)
            {
                double h = this->at(x, y);
                QColor color = QColor::fromRgbF(h, h, h);
                image.setPixelColor(x, y, color);
            }
        }
    }
    return image;
}

/**
 * toPixmap
 *
 * Returns the intensity as a pixmap for display.
 *
 * @returns QPixmap : The pixmap.
 */
QPixmap IntensityMap::toPixmap()
{
    qDebug("Converting Intensity map to QPixmap");
    return QPixmap::fromImage(this->toImage(false));
}

/**
 * scaled
 *
 * Scales the intensity map into a new value. This uses a QImage under the hood
 * to take advantage of Qt image scaling with linear interpolation.
 *
 * @param int width : The new width to scale to.
 * @param int height : The new height to scale to.
 *
 * @returns IntensityMap : The new updated intensity map.
 */
IntensityMap IntensityMap::scaled(int width, int height)
{
    QImage image = this->toImage().scaled(width, height);
    return IntensityMap(image, IntensityMap::AVERAGE);
}

/**
 * transform
 *
 * Returns a new intensity map using a tranformation function with a fixed value
 * by applying the function with the value to each pixel.
 *
 * @param [](double, double){} -> double : The function that applies the
 *                                         transformation.
 * @param double value : The fixed value used in the function.
 *
 * @returns IntensityMap : The new transformed intensity map.
 */
IntensityMap IntensityMap::transform(double func(double, double), double value)
{
    IntensityMap map;
    if (this->usingFill())
    {
        map = IntensityMap(this->width, this->height, func(this->_fill, value));
    }
    else
    {
        map = IntensityMap(this->width, this->height);
        for (int y = 0; y < this->height; y++)
            for (int x = 0; x < this->width; x++)
                map.append(func(this->at(x, y), value));
    }

    return map;
}

/**
 * transform
 *
 * Tranform the intensity map pixel by pixel by appling the provided function
 * with another intensity map.
 *
 * @param [](double, double){} -> double : The transformation function.
 * @param IntensityMap* map : The other map to apply the transformation with.
 *
 * @returns IntensityMap : The newly transformed function.
 */
IntensityMap IntensityMap::transform(double func(double, double),
                                     IntensityMap *map)
{
    Q_CHECK_PTR(map);
    IntensityMap out;
    if (this->usingFill() && map->usingFill())
    {
        out = IntensityMap(this->width, this->height,
                           func(this->_fill, map->at(0, 0)));
    }
    else
    {
        out = IntensityMap(this->width, this->height);
        for (int y = 0; y < this->height; y++)
            for (int x = 0; x < this->width; x++)
                out.append(func(this->at(x, y), map->at(x, y)));
    }

    return out;
}

/**
 * at
 *
 * Get the value at a specific index of the map. If the requested index is
 * beyond the bounds of the map the fill value is returned instead.
 *
 * @param int x : The column.
 * @param int y : The row.
 * @param bool clamp : Use the clamp value rather than the fill, default=false;
 *
 * @returns double : The intensity at the specified index.
 */
double IntensityMap::at(int x, int y, bool clamp_to)
{
    if (x < 0 || x >= this->width || y < 0 || y >= this->height)
    {
        if (clamp_to)
            return this->at(
                clamp(x, 0, this->width - 1),
                clamp(y, 0, this->height - 1),
                clamp_to);
        else
            return this->_fill;
    }

    int index = y * this->width + x;
    if (index >= (int)this->values.size())
        return this->_fill;

    return this->values[index];
}

/**
 * usingFill
 *
 * Returns true if the intensity map is using the fill value. i.e. no pixel
 * manipulation has occured, it is a solid value map.
 *
 * @returns bool : Whether or not the map is only using fill.
 */
bool IntensityMap::usingFill()
{
    return this->_use_fill;
}

/**
 * append
 *
 * Appends a value to the end of the values list of intensities.
 *
 * @param double value : The value to be appended.
 *
 * @returns bool : Whether the value was appended or not.
 */
bool IntensityMap::append(double value)
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
 * Overrides a specific intensity value at a the specififed index.
 *
 * @param int x : The column.
 * @param int y : The row.
 * @param double value : The value to be overriding with.
 *
 * @returns bool : Wether the value was overridden or not.
 */
bool IntensityMap::set(int x, int y, double value)
{
    if (x < 0 || x >= this->width || y < 0 || y >= this->height)
        return false;

    int index = y * this->width + x;
    this->_use_fill = false;
    if (index >= (int)this->values.size())
        for (int i = (int)this->values.size(); i <= index; i++)
            this->values.push_back(this->_fill);

    this->values[y * this->width + x] = value;
    return true;
}

/**
 * _saveImage
 *
 * Helper function used to convert a supplied QImage (or QPixmap that is casted
 * into a QImage) into an intensity map.
 *
 * @param QImage image : The image used to pull information from.
 * @param IntensityMap::Channel channel : The channel to create the map from.
 */
void IntensityMap::_saveImage(QImage image, IntensityMap::Channel channel)
{
    this->width = image.width();
    this->height = image.height();
    switch (channel)
    {
        // Use only the red channel
    case IntensityMap::RED:
        qDebug("Converting image with channel red");
        break;

        // Use only the green channel
    case IntensityMap::GREEN:
        qDebug("Converting image with channel green");
        break;

        // Use only the blue channel
    case IntensityMap::BLUE:
        qDebug("Converting image with channel blue");
        break;

        // Use only the blue channel
    case IntensityMap::ALPHA:
        qDebug("Converting image with channel alpha");
        break;

        // Average the red, green, and blue channels
    case IntensityMap::AVERAGE:
        qDebug("Converting image with channels averaged");
        break;

        // Select the smallest of the red, green, and blue channels
    case IntensityMap::MIN:
        qDebug("Converting image with minimum channel");
        break;

        // Select the largest of the red, green, and blue channels
    case IntensityMap::MAX:
        qDebug("Converting image with maximum channel");
        break;

    default:
        Q_UNREACHABLE();
        break;
    }
    for (int y = 0; y < this->height; y++)
    {
        for (int x = 0; x < this->width; x++)
        {
            double c, min, max;
            QColor color = image.pixelColor(x, y);
            switch (channel)
            {
                // Use only the red channel
            case IntensityMap::RED:
                this->values.push_back(color.redF());
                break;

                // Use only the green channel
            case IntensityMap::GREEN:
                this->values.push_back(color.greenF());
                break;

                // Use only the blue channel
            case IntensityMap::BLUE:
                this->values.push_back(color.blueF());
                break;

                // Use only the alpha channel
            case IntensityMap::ALPHA:
                this->values.push_back(color.alphaF());
                break;

                // Average the red, green, and blue channels
            case IntensityMap::AVERAGE:
                this->values.push_back((color.redF() + color.greenF() + color.blueF() + color.alphaF()) / 4.00);
                break;

                // Select the smallest of the red, green, and blue channels
            case IntensityMap::MIN:
                min = color.redF();
                c = color.greenF();
                if (c < min)
                    min = c;
                c = color.blueF();
                if (c < min)
                    min = c;
                c = color.alphaF();
                if (c < min)
                    min = c;
                this->values.push_back(min);
                break;

                // Select the largest of the red, green, and blue channels
            case IntensityMap::MAX:
                max = color.redF();
                c = color.greenF();
                if (c > max)
                    max = c;
                c = color.blueF();
                if (c > max)
                    max = c;
                c = color.alphaF();
                if (c > max)
                    max = c;
                this->values.push_back(max);
                break;

            default:
                Q_UNREACHABLE();
                break;
            }
        }
    }
}
