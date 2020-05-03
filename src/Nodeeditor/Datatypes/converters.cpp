#include "converters.h"

// Convert Intensity map to Vector Map
std::shared_ptr<QtNodes::NodeData>
IntensityToVectorMapConverter::operator()(std::shared_ptr<QtNodes::NodeData> data)
{
    std::shared_ptr<IntensityMapData> map = std::dynamic_pointer_cast<IntensityMapData>(data);
    IntensityMap intensity_map = map->intensityMap();
    VectorMap vector_map = VectorMap::fromIntensityMap(intensity_map);
    this->_out = std::make_shared<VectorMapData>(vector_map);
    return this->_out;
}

// Convert Vector map to Intensity Map
std::shared_ptr<QtNodes::NodeData>
VectorToIntensityMapConverter::operator()(std::shared_ptr<QtNodes::NodeData> data)
{
    std::shared_ptr<VectorMapData> map = std::dynamic_pointer_cast<VectorMapData>(data);
    IntensityMap intensity_map = map->vectorMap().toIntensityMap();
    this->_out = std::make_shared<IntensityMapData>(intensity_map);
    return this->_out;
}