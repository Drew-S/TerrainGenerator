#pragma once

#include <QPixmap>

#include <nodes/NodeDataModel>

#include "vectormap.h"
#include "intensitymap.h"

// Manages data transfer between nodes, manages Intensity maps
class IntensityMapData : public QtNodes::NodeData
{
public:
    IntensityMapData();
    IntensityMapData(IntensityMap const &intensity_map);

    // The type of data this is { QString: id, QString name}, name shows up on node
    QtNodes::NodeDataType type() const override;

    // Returns the stored colorMap
    IntensityMap intensityMap() const;

private:
    IntensityMap _intensity_map;
};

// Manages data transfer between nodes, manages Vector maps
class VectorMapData : public QtNodes::NodeData
{
public:
    VectorMapData();
    VectorMapData(VectorMap const &vector_map);

    // The type of data this is { QString: id, QString name}, name shows up on node
    QtNodes::NodeDataType type() const override;

    // Returns the stored colorMap
    VectorMap vectorMap() const;

private:
    VectorMap _vector_map;
};
