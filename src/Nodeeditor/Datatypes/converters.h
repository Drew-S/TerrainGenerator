#pragma once

#include <lib/nodeeditor/include/nodes/NodeData>

#include "pixmap.h"
#include "vectormap.h"
#include "intensitymap.h"

// Converts an intensity map shared_ptr container to a vector map
class IntensityToVectorMapConverter
{
public:
    std::shared_ptr<QtNodes::NodeData> operator()(std::shared_ptr<QtNodes::NodeData> data)
    {
        std::shared_ptr<IntensityMapData> map = std::dynamic_pointer_cast<IntensityMapData>(data);
        IntensityMap intensity_map = map->intensityMap();
        VectorMap vector_map = VectorMap::fromIntensityMap(intensity_map);
        this->_out = std::make_shared<VectorMapData>(vector_map);
        return this->_out;
    };

private:
    std::shared_ptr<VectorMapData> _out;
};

// Converts an vector map shared_ptr container to a intensity map
class VectorToIntensityMapConverter
{
public:
    std::shared_ptr<QtNodes::NodeData> operator()(std::shared_ptr<QtNodes::NodeData> data)
    {
        std::shared_ptr<VectorMapData> map = std::dynamic_pointer_cast<VectorMapData>(data);
        IntensityMap intensity_map = map->vectorMap().toIntensityMap();
        this->_out = std::make_shared<IntensityMapData>(intensity_map);
        return this->_out;
    };

private:
    std::shared_ptr<IntensityMapData> _out;
};
