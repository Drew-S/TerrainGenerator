#include "intensitymap.h"

IntensityMap::IntensityMap(){};

// Set initial size without data
IntensityMap::IntensityMap(int width, int height)
{
    this->width = width;
    this->height = height;
}

// Set size and values from a 1D vector array
IntensityMap::IntensityMap(int width, int height, std::vector<double> values)
{
    this->width = width;
    this->height = height;
    this->values = values;
}

// Create intensity map from image, using provided channel
IntensityMap::IntensityMap(QImage image, IntensityMap::Channel channel)
{
    this->_saveImage(image, channel);
}

// Create intensity map from pixmap, using provided channel
IntensityMap::IntensityMap(QPixmap image, IntensityMap::Channel channel)
{
    this->_saveImage(image.toImage(), channel);
}

IntensityMap::~IntensityMap() {}

// Convert the intensity map to an image
QImage IntensityMap::toImage()
{
    QImage image(this->width, this->height, QImage::Format_RGBA64);
    for (int x = 0; x < this->width; x++)
    {
        for (int y = 0; y < this->height; y++)
        {
            double h = this->at(x, y);
            QColor color = QColor::fromRgbF(h, h, h);
            image.setPixelColor(x, y, color);
        }
    }
    return image;
}

// Convert the intensity map to a pixmap
QPixmap IntensityMap::toPixmap() { return QPixmap::fromImage(this->toImage()); }

// Get a value a specific index (clamps to 0.00)
double IntensityMap::at(int x, int y)
{
    if (x < 0 || x >= this->width || y < 0 || y >= this->height)
        return 0.00;
    return this->values[y * this->width + x];
}

// Append a value to the end of the array
bool IntensityMap::append(double value)
{
    if (this->values.size() >= this->width * this->height)
        return false;
    this->values.push_back(value);
    return true;
}

// Set a specific value for an index
bool IntensityMap::set(int x, int y, double value)
{
    if (x < 0 || x >= this->width || y < 0 || y > this->height)
        return false;
    this->values[y * this->width + x] = value;
    return true;
}

// Used for convert image to intensity map
void IntensityMap::_saveImage(QImage image, IntensityMap::Channel channel)
{
    this->width = image.width();
    this->height = image.height();
    for (int x = 0; x < this->width; x++)
    {
        for (int y = 0; y < this->height; y++)
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

                // Average the red, green, and blue channels
            case IntensityMap::AVERAGE:
                this->values.push_back((color.redF() + color.greenF() + color.blueF()) / 3.00);
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
                this->values.push_back(max);
                break;
            }
        }
    }
}