#pragma once

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <vector>

#include <QImage>
#include <QColor>
#include <QThread>
#include <QMutex>
#include <QObject>

#include "../../Datatypes/intensitymap.h"

// Generates the normal map in a separate thread
class NormalWorker : public QObject
{
    Q_OBJECT
public:
    // Set variables that are needed, must be called after the worker is in a different thread
    void set(IntensityMap *height_map);

public slots:
    // Generate the normal map
    void generate();

signals:
    // Progress updates
    void started();
    void progress(int perc);
    void done(QImage const &normal_map);

private:
    // Helper function
    double _getHeightIntensity(int x, int y, IntensityMap *map);
    IntensityMap *_height_map = nullptr;
};

// Class to set parameters and generate a normal map from a supplied height map
class NormalMapGenerator : public QObject
{
    Q_OBJECT
public:
    NormalMapGenerator();
    NormalMapGenerator(IntensityMap height_map);

    // Generates the normal map
    // TODO: Meant to be threaded later on
    void generate();

    // Update the reference height map
    void setImage(IntensityMap height_map);

    // Get the generated normal map image
    QImage toImage();

signals:
    void done();
    void progress(int perc);
    void started();

public slots:
    void normalDone(QImage const &normal_map);

private:
    // Get the intensity (height) at a specific pixel
    // double _getHeightIntensity(int x, int y);

    // Normal map size
    int _width;
    int _height;
    int _gen_size;

    QThread _normal_thread;

    // Reference to maps
    QImage _normal_map;

    IntensityMap _height_map;
};