#include "normal.h"

#include <QDebug>

static double KERNEL_X[3][3] = {
    {-1.00, 0.00, 1.00},
    {-2.00, 0.00, 2.00},
    {-1.00, 0.00, 1.00}};

static double KERNEL_Y[3][3] = {
    {-1.00, -2.00, -1.00},
    {0.00, 0.00, 0.00},
    {1.00, 2.00, 1.00}};

// Constructor with a provided height map
NormalMapGenerator::NormalMapGenerator(VectorMap *height_map)
{
    this->setImage(height_map);
}

// Set/update the reference height map
void NormalMapGenerator::setImage(VectorMap *height_map)
{
    this->_height_map = height_map;
    this->_width = this->_height_map->width;
    this->_height = this->_height_map->height;
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
            // Neighbouring pixel values
            // tl  t  tr
            //  l  o   r
            // bl  b  br
            double matrix[3][3] = {
                {this->_getHeightIntensity(x - 1, y - 1),
                 this->_getHeightIntensity(x, y - 1),
                 this->_getHeightIntensity(x + 1, y - 1)},
                {this->_getHeightIntensity(x - 1, y),
                 0.00,
                 this->_getHeightIntensity(x + 1, y)},
                {this->_getHeightIntensity(x - 1, y + 1),
                 this->_getHeightIntensity(x, y + 1),
                 this->_getHeightIntensity(x + 1, y + 1)}};

            // | a b c |   | 1 2 3 |
            // | d e f | o | 4 5 6 | = a1 + b2 + c3 + d4 + e5 + f6 + g7 + h8 + i9
            // | g h i |   | 7 8 9 |
            double x_prime = KERNEL_X[0][0] * matrix[0][0] + KERNEL_X[0][1] * matrix[0][1] + KERNEL_X[0][2] * matrix[0][2] +
                             KERNEL_X[1][0] * matrix[1][0] + KERNEL_X[1][1] * matrix[1][1] + KERNEL_X[1][2] * matrix[1][2] +
                             KERNEL_X[2][0] * matrix[2][0] + KERNEL_X[2][1] * matrix[2][1] + KERNEL_X[2][2] * matrix[2][2];

            double y_prime = KERNEL_Y[0][0] * matrix[0][0] + KERNEL_Y[0][1] * matrix[0][1] + KERNEL_Y[0][2] * matrix[0][2] +
                             KERNEL_Y[1][0] * matrix[1][0] + KERNEL_Y[1][1] * matrix[1][1] + KERNEL_Y[1][2] * matrix[1][2] +
                             KERNEL_Y[2][0] * matrix[2][0] + KERNEL_Y[2][1] * matrix[2][1] + KERNEL_X[2][2] * matrix[2][2];

            // TODO: Add blur to smooth jaggedness

            double mag = glm::sqrt(x_prime * x_prime + y_prime * y_prime);

            // Get the normal vector, adjusted for [-1, 1] range to color range [0, 1]
            glm::dvec3 n = glm::normalize((glm::dvec3(-x_prime, -y_prime, mag) + 1.00) / 2.00);

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
double
NormalMapGenerator::_getHeightIntensity(int x, int y)
{
    if (x < 0 || y < 0 || x >= this->_width || y >= this->_height)
        return 0.00;
    double height = this->_height_map->at(x, y).z;

    // TODO: add strength control element
    double strength = 25.00;
    return height * strength;
}