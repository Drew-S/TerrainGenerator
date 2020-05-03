#pragma once

#include <vector>

#include <QImage>
#include <QPixmap>
#include <QColor>

#include <glm/vec3.hpp>

#include "intensitymap.h"

// Vector map is a 2D "image" of 3D vectors in high precision doubles
class VectorMap
{
public:
    // Create map from a variety of sources or to generate
    VectorMap();
    VectorMap(int width, int height);
    VectorMap(int width, int height, std::vector<glm::dvec3> values);
    VectorMap(QImage image);
    VectorMap(QPixmap image);
    ~VectorMap();

    // Converters to create from/to an intensity map
    IntensityMap toIntensityMap();
    static VectorMap fromIntensityMap(IntensityMap &map);

    // Return an image of the vector map
    QImage toImage();

    // Return a pixmap of the vector map
    QPixmap toPixmap();

    // Get a specific value
    glm::dvec3 at(int x, int y);

    // Append a value (for filling with generated data) (bool whether can/successful)
    bool append(glm::dvec3 value);

    // Set a specific pixel (bool whether can/successful)
    bool set(int x, int y, glm::dvec3 value);

    // Storage of the vector map data
    int width;
    int height;
    std::vector<glm::dvec3> values;

private:
    // Used to convert image to vector map
    void _saveImage(QImage image);
};
