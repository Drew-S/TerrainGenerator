#pragma once

#include <vector>

#include <QImage>
#include <QPixmap>

/**
 * IntensityMap
 *
 * Houses a 2 dimensional array (internally stored in 1 dimension) a list of 
 * doubles that create a mono-coloured image or height map.
 */
class IntensityMap
{
public:
    // Used for deciding what channel to use when converting a
    // Color image/vector map
    enum Channel
    {
        RED,
        GREEN,
        BLUE,
        ALPHA,
        AVERAGE,
        MIN,
        MAX
    };

    // Create empty map
    IntensityMap();
    
    // Create a map with a size (defaults to 1.00 fill)
    IntensityMap(int width, int height);

    // Create a map with a size and a fill value
    IntensityMap(int width, int height, double fill);

    // Createa a map with a size and mapped array of values (pixels)
    IntensityMap(int width, int height, std::vector<double> values);

    // Create a map from an image
    IntensityMap(QImage image, IntensityMap::Channel channel);

    // Create a map from a pixmap
    IntensityMap(QPixmap image, IntensityMap::Channel channel);

    // Return an image of the intensity map
    QImage toImage(bool print_qimage = true);

    // Return a pixmap of the intensity map
    QPixmap toPixmap();

    // Return intensity map scaled via image (linear interpolation)
    IntensityMap scaled(int width, int height);

    // Apply a transformation function on a per pixel basis
    //                                 pixel,  value
    IntensityMap transform(double func(double, double), double value);
    IntensityMap transform(double func(double, double), IntensityMap *map);

    // Get a specific value
    double at(int x, int y);

    // Check if the map is using a solid fill color (all pixels the same)
    bool usingFill();

    // Append a value (for filling with generated data) (bool whether can/successful)
    bool append(double value);

    // Set a specific pixel (bool whether can/successful)
    bool set(int x, int y, double value);

    // Storage of the intensity map data
    int width;
    int height;
    std::vector<double> values;

private:
    // Used to convert image to intensity map
    void _saveImage(QImage image, IntensityMap::Channel channel);
    double _fill = 0.00;
    bool _use_fill = false;
};