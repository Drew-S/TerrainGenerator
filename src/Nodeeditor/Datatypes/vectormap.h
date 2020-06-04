#pragma once

#include <vector>

#include <QColor>
#include <QImage>
#include <QPixmap>

#include <glm/vec4.hpp>

#include "intensitymap.h"

/**
 * VectorMap
 *
 * Houses a 2 dimensional array of (internally as a 1 dimensional array) of 
 * glm::dev4 (4 value double vector) a fourth dimension version of intensity
 * map.
 */
class VectorMap
{
public:
    // Modes for converting from an intensity map.
    enum ColorMode
    {
        APPLY,
        OVERRIDE_COLOR,
        OVERRIDE_MAP,
        MASK,
        MASK_ALPHA
    };

    // Create a new map
    VectorMap();
    
    // Create a vector map with a size and default fill value.
    VectorMap(int width, int height);

    // Create a vector map with a size and a set fill value.
    VectorMap(int width, int height, glm::dvec4 fill);

    // Create a vector map with a size and a set of mapped values.
    VectorMap(int width, int height, std::vector<glm::dvec4> values);

    // Create a vector map from a supplied image.
    VectorMap(QImage image);

    // Create a vector map from a supplied pixmap.
    VectorMap(QPixmap image);

    // Converter to an intensity with a specified channel.
    IntensityMap toIntensityMap(IntensityMap::Channel channel
                                = IntensityMap::BLUE);

    // Convert from an intensity map using the applied mode a color override.
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
    VectorMap transform(glm::dvec4 func(glm::dvec4, glm::dvec4),
                        glm::dvec4 value);

    VectorMap transform(glm::dvec4 func(glm::dvec4, glm::dvec4),
                        VectorMap *map);

    // Get a specific value
    glm::dvec4 at(int x, int y);

    // Append a value (for filling with generated data) (bool whether
    // can/successful)
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
