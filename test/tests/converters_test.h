#pragma once

#include <QtTest>

#include <vector>

#include <nodes/NodeData>

#include "../src/Nodeeditor/Datatypes/converters.h"

#include "../src/Nodeeditor/Datatypes/pixmap.h"
#include "../src/Nodeeditor/Datatypes/vectormap.h"
#include "../src/Nodeeditor/Datatypes/intensitymap.h"

#include <glm/vec4.hpp>

class IntensityToVectorMapConverter_Test : public QObject
{
    Q_OBJECT
private slots:
    void converter()
    {
        std::vector<double> data{0.50};
        IntensityMap from(1, 1, data);
        std::shared_ptr<IntensityMapData> from_data = std::make_shared<IntensityMapData>(from);

        IntensityToVectorMapConverter converter;
        std::shared_ptr<QtNodes::NodeData> to_data = converter(from_data);
        VectorMap to = std::dynamic_pointer_cast<VectorMapData>(to_data)->vectorMap();

        QCOMPARE(to.at(0, 0), glm::dvec4(0.50, 0.50, 0.50, 0.50));
    };
};

class VectorToIntensityMapConverter_Test : public QObject
{
    Q_OBJECT
private slots:
    void converter()
    {
        std::vector<glm::dvec4> data{glm::dvec4(1.00, 2.00, 3.00, 4.00)};
        VectorMap from(1, 1, data);
        std::shared_ptr<VectorMapData> from_data = std::make_shared<VectorMapData>(from);

        VectorToIntensityMapConverter converter;
        std::shared_ptr<QtNodes::NodeData> to_data = converter(from_data);
        IntensityMap to = std::dynamic_pointer_cast<IntensityMapData>(to_data)->intensityMap();

        QCOMPARE(to.at(0, 0), 3.00);
    };
};