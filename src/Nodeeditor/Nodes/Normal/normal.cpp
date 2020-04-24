#include "normal.h"

#include <QDebug>

// Constructor with a provided height map
NormalMapGenerator::NormalMapGenerator(QImage *height_map)
{
    this->setImage(height_map);
}

// Set/update the reference height map
void NormalMapGenerator::setImage(QImage *height_map)
{
    this->_height_map = height_map;
    this->_width = this->_height_map->width();
    this->_height = this->_height_map->height();
}

// Generate the normal map (uses sobel filtering)
void NormalMapGenerator::generate()
{
    // Normal map image
    QImage normal(this->_width, this->_height, QImage::Format_RGBA64);

    // Loop over all the pixels
    for (int x = 0; x < this->_width; x++)
    {
        for (int y = 0; y < this->_height; y++)
        {
            // For a pixel o get neighbour pixel heights
            // tl  t  tr
            //  l  o   r
            // bl  b  br
            double tl = this->_getHeightIntensity(x - 1, y - 1);
            double t = this->_getHeightIntensity(x, y - 1);
            double tr = this->_getHeightIntensity(x + 1, y - 1);
            double r = this->_getHeightIntensity(x + 1, y);
            double br = this->_getHeightIntensity(x + 1, y + 1);
            double b = this->_getHeightIntensity(x, y + 1);
            double bl = this->_getHeightIntensity(x - 1, y + 1);
            double l = this->_getHeightIntensity(x - 1, y);

            // Sobel kernel
            // tl  2t  tr | top    = sum of row_0
            // 2l   o  2r | bottom = sum of row_2
            // bl  2b  br | left   = sum of col_0
            //              right  = sum of col_2
            double top = tl + t * 2.00 + tr;
            double bottom = bl + b * 2.00 + br;
            double left = tl + l * 2.00 + bl;
            double right = tr + r * 2.00 + br;

            // TODO: Add control element
            double strength = 1.00;

            // Get the normal vector, adjusted for [-1, 1] range to color range [0, 1]
            glm::dvec3 n = glm::normalize((glm::dvec3(right - left, bottom - top, strength) + 1.00) / 2.00);

            // Create color and apply to the normal image
            QColor color = QColor::fromRgbF(n.x, n.y, n.z);
            normal.setPixelColor(x, y, color);
        }
    }

    // Save normal image
    this->_normal_map = normal;
}

// Return the generated normal map
QImage NormalMapGenerator::toImage()
{
    return this->_normal_map;
}

// Gets the height intensity for a pixel, if x goes beyond borders,
// it is clamped back to the corner (other options could be to wrap)
double NormalMapGenerator::_getHeightIntensity(int x, int y)
{
    if (x < 0)
        x = 0;

    if (y < 0)
        y = 0;

    if (x >= this->_width)
        x = this->_width - 1;

    if (y >= this->_height)
        y = this->_height - 1;

    // TODO: add strength control element
    double strength = 25.00;
    return this->_height_map->pixelColor(x, y).blueF() * strength;
}