#include "converters.h"

#include <QDebug>

// Convert Intensity map to Vector Map
std::shared_ptr<QtNodes::NodeData>
IntensityToVectorMapConverter::operator()(std::shared_ptr<QtNodes::NodeData> data)
{
    qDebug("Converting Intensity Map connection to Vector Map");
    std::shared_ptr<IntensityMapData> map = std::dynamic_pointer_cast<IntensityMapData>(data);
    if (map)
    {
        IntensityMap intensity_map = map->intensityMap();
        VectorMap vector_map = VectorMap::fromIntensityMap(intensity_map);
        this->_out = std::make_shared<VectorMapData>(vector_map);
    }
    else
    {
        // NOTE: For future reference. The if/else and reset is required otherwise disconnecting/deleting
        //       nodes with converters will crash program.
        this->_out.reset();
    }
    return this->_out;
}

// Convert Vector map to Intensity Map
std::shared_ptr<QtNodes::NodeData>
VectorToIntensityMapConverter::operator()(std::shared_ptr<QtNodes::NodeData> data)
{
    qDebug("Converting Vector Map connection to Intensity Map");
    std::shared_ptr<VectorMapData> map = std::dynamic_pointer_cast<VectorMapData>(data);
    if (map)
    {
        IntensityMap intensity_map = map->vectorMap().toIntensityMap();
        this->_out = std::make_shared<IntensityMapData>(intensity_map);
    }
    else
    {
        this->_out.reset();
    }
    return this->_out;
}
