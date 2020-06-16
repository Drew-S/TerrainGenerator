#pragma once

#include <vector>

#include <QColor>
#include <QImage>
#include <QMutex>
#include <QObject>
#include <QThread>

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

#include "../../Datatypes/intensitymap.h"

/**
 * NormalWorker
 * 
 * Worker class for generating the normal map in its own dedicated thread.
 */
class NormalWorker : public QObject
{
    Q_OBJECT
public:
    // Set variables that are needed, must be called after the worker is in a
    // different thread
    void set(IntensityMap *height_map);

public slots:
    // Generate the normal map
    void generate();

    // Stop generation
    void stop();

signals:
    // Progress updates
    void started();
    void progress(int perc);
    void done(QImage const &normal_map);
    void stopped();

private:
    // Helper function
    double _getHeightIntensity(int x, int y, IntensityMap *map);
    IntensityMap *_height_map = nullptr;

    // Flag for interuption
    bool _run = false;
};

/**
 * NormalMapGenerator
 * 
 * Class that is used to generate the normal map.
 */
class NormalMapGenerator : public QObject
{
    Q_OBJECT
public:
    // Create a normal map generator
    NormalMapGenerator();
    // Safely delete the object
    ~NormalMapGenerator();

    // Create a ne generator with a set height map.
    NormalMapGenerator(IntensityMap height_map);

    // Generates the normal map
    void generate();

    // Update the reference height map
    void setImage(IntensityMap height_map);

    // Get the generated normal map image
    QImage toImage();

signals:
    // Progress signals.
    void done();
    void progress(int perc);
    void started();

public slots:
    void normalDone(QImage const &normal_map);

private:
    // Normal map size
    int _width;
    int _height;
    int _gen_size;

    // The thread the generation occurs in
    QThread _normal_thread;
    NormalWorker *_worker = nullptr;

    // The resulting normal map.
    QImage _normal_map;

    // The input height map
    IntensityMap _height_map;
};