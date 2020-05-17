#pragma once

#include <QtTest>

#include <vector>

#include "../../src/Nodeeditor/Datatypes/pixmap.h"

#include "../../src/Nodeeditor/Datatypes/vectormap.h"
#include "../../src/Nodeeditor/Datatypes/intensitymap.h"

#include <nodes/NodeDataModel>

#include <glm/vec4.hpp>

#define ZERO glm::dvec4(0.00, 0.00, 0.00, 0.00)
#define VEC_1000 glm::dvec4(1.00, 0.00, 0.00, 0.00)
#define VEC_0100 glm::dvec4(0.00, 1.00, 0.00, 0.00)
#define VEC_0010 glm::dvec4(0.00, 0.00, 1.00, 0.00)
#define VEC_0001 glm::dvec4(0.00, 0.00, 0.00, 1.00)

class IntensityMapData_Test : public QObject
{
    Q_OBJECT
private slots:
    void intensityMap()
    {
        std::vector<double> data = {1.00, 2.00, 3.00, 4.00};
        IntensityMapData map_data(IntensityMap(2, 2, data));

        IntensityMap map(2, 2, data);
        IntensityMap data_map = map_data.intensityMap();
        for (int y = 0; y < map.width; y++)
            for (int x = 0; x < map.height; x++)
                QCOMPARE(map.at(x, y), data_map.at(x, y));
    };
    void type()
    {
        QtNodes::NodeDataType type = IntensityMapData().type();
        QtNodes::NodeDataType compare = {"IntensityMap", "sampler2D"};
        QCOMPARE(type.id, compare.id);
        QCOMPARE(type.name, compare.name);
    };
};

class VectorMapData_Test : public QObject
{
    Q_OBJECT
private slots:
    void vectorMap()
    {
        std::vector<glm::dvec4> data{VEC_1000, VEC_0100, VEC_0010, VEC_0001};
        VectorMapData map_data(VectorMap(2, 2, data));

        VectorMap map(2, 2, data);
        VectorMap data_map = map_data.vectorMap();
        for (int y = 0; y < map.width; y++)
            for (int x = 0; x < map.height; x++)
                QCOMPARE(map.at(x, y), data_map.at(x, y));
    };
    void type()
    {
        QtNodes::NodeDataType type = VectorMapData().type();
        QtNodes::NodeDataType compare = {"VectorMap", "sampler2D"};
        QCOMPARE(type.id, compare.id);
        QCOMPARE(type.name, compare.name);
    };
};