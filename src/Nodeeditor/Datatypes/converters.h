#pragma once

#include <nodes/NodeData>

#include "pixmap.h"
#include "vectormap.h"
#include "intensitymap.h"

// Converts an intensity map shared_ptr container to a vector map
class IntensityToVectorMapConverter
{
public:
    std::shared_ptr<QtNodes::NodeData> operator()(std::shared_ptr<QtNodes::NodeData> data);

private:
    std::shared_ptr<VectorMapData> _out;
};

// Converts an vector map shared_ptr container to a intensity map
class VectorToIntensityMapConverter
{
public:
    std::shared_ptr<QtNodes::NodeData> operator()(std::shared_ptr<QtNodes::NodeData> data);

private:
    std::shared_ptr<IntensityMapData> _out;
};
