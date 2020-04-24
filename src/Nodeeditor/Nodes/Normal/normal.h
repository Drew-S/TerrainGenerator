#pragma once

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <vector>

#include <QImage>

// TODO: make a worker to utilize QThread's
// Class to set parameters and generate a normal map from a supplied height map
class NormalMapGenerator
{
public:
    NormalMapGenerator(){};
    NormalMapGenerator(QImage *height_map);

    // Generates the normal map
    // TODO: Meant to be threaded later on
    void generate();

    // Update the reference height map
    void setImage(QImage *height_map);

    // Get the generated normal map image
    QImage toImage();

private:
    // Get the intensity (height) at a specific pixel
    double _getHeightIntensity(int x, int y);

    // Normal map size
    int _width;
    int _height;

    // Reference to maps
    QImage _normal_map;
    QImage *_height_map;
};