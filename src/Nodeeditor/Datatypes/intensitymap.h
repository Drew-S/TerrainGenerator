#pragma once

#include <vector>

#include <QImage>
#include <QPixmap>

// Data type for a two dimensional "image" for double values
// Data is a "greyscale"
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
        AVERAGE,
        MIN,
        MAX
    };

    // Create map from a variety of sources or to generate
    IntensityMap();
    IntensityMap(int width, int height);
    IntensityMap(int width, int height, double fill);
    IntensityMap(int width, int height, std::vector<double> values);
    IntensityMap(QImage image, IntensityMap::Channel channel);
    IntensityMap(QPixmap image, IntensityMap::Channel channel);

    ~IntensityMap();

    // Return an image of the intensity map
    QImage toImage(bool print_qimage = true);

    // Return a pixmap of the intensity map
    QPixmap toPixmap();

    // Return intensity map scaled via image (linear interpolation)
    IntensityMap scaled(int width, int height);

    // Get a specific value
    double at(int x, int y);

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
};