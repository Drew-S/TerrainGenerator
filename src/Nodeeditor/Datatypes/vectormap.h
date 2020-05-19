#pragma once

#include <vector>

#include <QImage>
#include <QPixmap>
#include <QColor>

#include <glm/vec4.hpp>

#include "intensitymap.h"

// Vector map is a 2D "image" of 3D vectors in high precision doubles
class VectorMap
{
public:
    enum ColorMode
    {
        APPLY,
        OVERRIDE_COLOR,
        OVERRIDE_MAP,
        MASK,
        MASK_ALPHA
    };
    // Create map from a variety of sources or to generate
    VectorMap();
    VectorMap(int width, int height);
    VectorMap(int width, int height, glm::dvec4 fill);
    VectorMap(int width, int height, std::vector<glm::dvec4> values);
    VectorMap(QImage image);
    VectorMap(QPixmap image);
    ~VectorMap();

    // Converters to create from/to an intensity map
    IntensityMap toIntensityMap();
    static VectorMap fromIntensityMap(
        IntensityMap &map,
        glm::dvec4 color = glm::dvec4(1.00, 1.00, 1.00, 1.00),
        VectorMap::ColorMode mode = VectorMap::APPLY);

    // Return an image of the vector map
    QImage toImage(bool print_qimage = true);

    // Return a pixmap of the vector map
    QPixmap toPixmap();

    // Return vector map scaled via image (linear interpolation)
    VectorMap scaled(int width, int height);

    // Check if the map is using a solid fill color (all pixels the same)
    bool usingFill();

    // Transform a vector map using a provided lambda
    //                                  pixel       value
    VectorMap transform(glm::dvec4 func(glm::dvec4, glm::dvec4), glm::dvec4 value);
    VectorMap transform(glm::dvec4 func(glm::dvec4, glm::dvec4), VectorMap *map);

    // Get a specific value
    glm::dvec4 at(int x, int y);

    // Append a value (for filling with generated data) (bool whether can/successful)
    bool append(glm::dvec4 value);

    // Set a specific pixel (bool whether can/successful)
    bool set(int x, int y, glm::dvec4 value);

    // Storage of the vector map data
    int width;
    int height;
    std::vector<glm::dvec4> values;

private:
    // Used to convert image to vector map
    void _saveImage(QImage image);
    glm::dvec4 _fill{0.00, 0.00, 0.00, 0.00};
    bool _use_fill = false;
};
