#pragma once

#include <QPixmap>

#include <lib/nodeeditor/include/nodes/NodeDataModel>

#include "vectormap.h"
#include "intensitymap.h"

// Manages data transfer between nodes, manages Intensity maps
class IntensityMapData : public QtNodes::NodeData
{
public:
    IntensityMapData(){};
    IntensityMapData(QPixmap const &pixmap)
    {
        this->_intensity_map = IntensityMap(pixmap, IntensityMap::AVERAGE);
    };
    IntensityMapData(IntensityMap const &intensity_map) : _intensity_map(intensity_map){};

    // The type of data this is { QString: id, QString name}, name shows up on node
    QtNodes::NodeDataType type() const override { return {"IntensityMap", "sampler2D"}; };

    // Returns the stored colorMap
    IntensityMap intensityMap() const { return this->_intensity_map; };

private:
    IntensityMap _intensity_map;
};

// Manages data transfer between nodes, manages Vector maps
class VectorMapData : public QtNodes::NodeData
{
public:
    VectorMapData(){};
    VectorMapData(QPixmap const &pixmap)
    {
        this->_vector_map = VectorMap(pixmap);
    };
    VectorMapData(VectorMap const &vector_map) : _vector_map(vector_map){};

    // The type of data this is { QString: id, QString name}, name shows up on node
    QtNodes::NodeDataType type() const override { return {"VectorMap", "sampler2D"}; };

    // Returns the stored colorMap
    VectorMap vectorMap() const { return this->_vector_map; };

private:
    VectorMap _vector_map;
};
