#include "pixmap.h"

// Create intensity connector containers from a variety of sources
IntensityMapData::IntensityMapData() {}
IntensityMapData::IntensityMapData(QPixmap const &pixmap)
{
    this->_intensity_map = IntensityMap(pixmap, IntensityMap::AVERAGE);
}
IntensityMapData::IntensityMapData(IntensityMap const &intensity_map)
    : _intensity_map(intensity_map) {}

// Returns the type of this connector
QtNodes::NodeDataType IntensityMapData::type() const
{
    return {"IntensityMap", "sampler2D"};
}

// Get the underlying data container
IntensityMap IntensityMapData::intensityMap() const
{
    return this->_intensity_map;
}

// Create vector connector containers from a variety of sources
VectorMapData::VectorMapData(){};
VectorMapData::VectorMapData(QPixmap const &pixmap)
{
    this->_vector_map = VectorMap(pixmap);
}
VectorMapData::VectorMapData(VectorMap const &vector_map) : _vector_map(vector_map){};

// Returns the type of this connector
QtNodes::NodeDataType VectorMapData::type() const
{
    return {"VectorMap", "sampler2D"};
}
// Get the underlying data container
VectorMap VectorMapData::vectorMap() const
{
    return this->_vector_map;
}