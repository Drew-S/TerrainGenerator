#pragma once

#include <QPixmap>

#include <nodes/NodeDataModel>

#include "intensitymap.h"
#include "vectormap.h"

/**
 * IntensityMapData
 * 
 * Class that wraps an intensity map for transport between nodes in a connection
 */
class IntensityMapData : public QtNodes::NodeData
{
public:
    // Create a new empty map data
    IntensityMapData();

    // Create a map data from a map
    IntensityMapData(IntensityMap const &intensity_map);

    // The type of data this is { QString: id, QString name}, name shows up on
    // node
    QtNodes::NodeDataType type() const override;

    // Returns the stored colorMap
    IntensityMap intensityMap() const;

private:
    IntensityMap _intensity_map;
};

/**
 * VectorMapData
 * 
 * Class that wraps an vector map for transport between nodes in a connection
 */
class VectorMapData : public QtNodes::NodeData
{
public:
    // Create a new empty map data
    VectorMapData();

    // Create a map data from a map
    VectorMapData(VectorMap const &vector_map);

    // The type of data this is { QString: id, QString name}, name shows up on
    // node
    QtNodes::NodeDataType type() const override;

    // Returns the stored colorMap
    VectorMap vectorMap() const;

private:
    VectorMap _vector_map;
};
