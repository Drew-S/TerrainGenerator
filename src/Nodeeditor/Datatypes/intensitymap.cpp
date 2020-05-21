#include "intensitymap.h"

#include <QDebug>

IntensityMap::IntensityMap(){};

// Set initial size without data
IntensityMap::IntensityMap(int width, int height)
{
    qDebug("Creating Intensity Map with only size set: (%dx%d)", width, height);
    this->width = width;
    this->height = height;
}

// Set initial size with a fill value
IntensityMap::IntensityMap(int width, int height, double fill)
{
    qDebug("Creating Intensity Map with only size set: (%dx%d)", width, height);
    this->width = width;
    this->height = height;
    this->_fill = fill;
    this->_use_fill = true;
}

// Set size and values from a 1D vector array
IntensityMap::IntensityMap(int width, int height, std::vector<double> values)
{
    qDebug("Creating Intensity Map with data: (%dx%d), %d pixels set", width, height, width * height);
    this->width = width;
    this->height = height;
    this->values = values;
}

// Create intensity map from image, using provided channel
IntensityMap::IntensityMap(QImage image, IntensityMap::Channel channel)
{
    qDebug("Creating Intensity Map from QImage");
    this->_saveImage(image, channel);
}

// Create intensity map from pixmap, using provided channel
IntensityMap::IntensityMap(QPixmap image, IntensityMap::Channel channel)
{
    qDebug("Creating Intensity Map from QPixmap");
    this->_saveImage(image.toImage(), channel);
}

IntensityMap::~IntensityMap() {}

// Convert the intensity map to an image
QImage IntensityMap::toImage(bool print_qimage)
{
    if (print_qimage)
        qDebug("Converting Intensity map to QImage");
    QImage image(this->width, this->height, QImage::Format_RGBA64);
    if (this->usingFill())
    {
        image.fill(QColor::fromRgbF(this->_fill, this->_fill, this->_fill, this->_fill));
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

// Convert the intensity map to a pixmap
QPixmap IntensityMap::toPixmap()
{
    qDebug("Converting Intensity map to QPixmap");
    return QPixmap::fromImage(this->toImage(false));
}

// Get a value at a specific index, returns the fill color if beyond bounds
// or empty pixels
IntensityMap IntensityMap::scaled(int width, int height)
{
    QImage image = this->toImage().scaled(width, height);
    return IntensityMap(image, IntensityMap::AVERAGE);
}

// Apply a transformation function on a per pixel basis
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

// Apply a transformation function on a per pixel basis
IntensityMap IntensityMap::transform(double func(double, double), IntensityMap *map)
{
    IntensityMap out;
    if (this->usingFill() && map->usingFill())
    {
        out = IntensityMap(this->width, this->height, func(this->_fill, map->at(0, 0)));
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

// Get a value a specific index (clamps to 0.00)
double IntensityMap::at(int x, int y)
{
    if (x < 0 || x >= this->width || y < 0 || y >= this->height)
        return this->_fill;

    int index = y * this->width + x;
    if (index >= (int)this->values.size())
        return this->_fill;

    return this->values[index];
}

bool IntensityMap::usingFill()
{
    return this->_use_fill;
}

// Append a value to the end of the array
bool IntensityMap::append(double value)
{
    if ((int)this->values.size() >= this->width * this->height)
        return false;

    this->_use_fill = false;
    this->values.push_back(value);
    return true;
}

// Set a specific value for an index
bool IntensityMap::set(int x, int y, double value)
{
    if (x < 0 || x >= this->width || y < 0 || y >= this->height)
        return false;

    this->_use_fill = false;

    this->values[y * this->width + x] = value;
    return true;
}

// Used for convert image to intensity map
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
            }
        }
    }
}
