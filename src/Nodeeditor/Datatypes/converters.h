#pragma once

#include <nodes/NodeData>

#include "intensitymap.h"
#include "pixmap.h"
#include "vectormap.h"

/**
 * IntensityToVectorMapConverter
 *
 * This is a converter class that converts an intensity map into vector map
 * between an intensity map output port and a vector map input port
 */
class IntensityToVectorMapConverter
{
public:
    // Converts intensity map to vector map
    std::shared_ptr<QtNodes::NodeData>
    operator()(std::shared_ptr<QtNodes::NodeData> data);

private:
    // The output vector map pointer
    std::shared_ptr<VectorMapData> _out;
};

/**
 * VectorToIntensityMapConverter
 *
 * This is a converter class that converts an vector map into intensity map
 * between an vector map output port and a intensity map input port
 */
class VectorToIntensityMapConverter
{
public:
    // Converts a vector map to intensity map
    std::shared_ptr<QtNodes::NodeData>
    operator()(std::shared_ptr<QtNodes::NodeData> data);

private:
    // The output intensity map pointer
    std::shared_ptr<IntensityMapData> _out;
};
