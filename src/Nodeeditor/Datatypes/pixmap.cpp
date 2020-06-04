#include "pixmap.h"

#include <QDebug>

/**
 * IntensityMapData
 * 
 * Creates a new intensity map data.
 */
IntensityMapData::IntensityMapData() {}

/**
 * IntensityMapData
 * 
 * Creates a new intensity map data using the provided intensity map.
 * 
 * @param IntenistyMap const& intensity_map : The housed intensity map.
 */
IntensityMapData::IntensityMapData(IntensityMap const &intensity_map)
    : _intensity_map(intensity_map)
{
    qDebug("Creating Intensity Map connector data with Intensity Map");
}

/**
 * type
 * 
 * Returns the type that the container is. This is used to determine what ports
 * can accept this data and the label on those ports.
 * 
 * @returns QtNodes::NodeDataType : The type of data this is.
 */
QtNodes::NodeDataType IntensityMapData::type() const
{
    return {"IntensityMap", "mono"};
}

/**
 * intensityMap
 * 
 * Returns the housed intensity map.
 * 
 * @returns IntensityMap : The intensity map.
 */
IntensityMap IntensityMapData::intensityMap() const
{
    return this->_intensity_map;
}

/**
 * VectorMapData
 * 
 * Creates a new vector map data.
 */
VectorMapData::VectorMapData() {}

/**
 * VectorMapData
 * 
 * Creates a new vector map data using the provided vector map.
 * 
 * @param IntenistyMap const& vector_map : The housed vector map.
 */
VectorMapData::VectorMapData(VectorMap const &vector_map)
    : _vector_map(vector_map)
{
    qDebug("Creating Vector Map connector data with Vector Map");
}

/**
 * type
 * 
 * Returns the type that the container is. This is used to determine what ports
 * can accept this data and the label on those ports.
 * 
 * @returns QtNodes::NodeDataType : The type of data this is.
 */
QtNodes::NodeDataType VectorMapData::type() const
{
    return {"VectorMap", "rgba"};
}

/**
 * vectorMap
 * 
 * Returns the housed vector map.
 * 
 * @returns VectorMap : The vector map.
 */
VectorMap VectorMapData::vectorMap() const
{
    return this->_vector_map;
}
