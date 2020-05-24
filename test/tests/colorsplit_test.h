#pragma once

#include <QtTest>

#include <nodes/NodeData>

#include <glm/vec4.hpp>

#include "../src/Nodeeditor/Nodes/colorsplit.h"
#include "../src/Nodeeditor/Datatypes/pixmap.h"
#include "../src/Nodeeditor/Datatypes/vectormap.h"
#include "../src/Nodeeditor/Datatypes/intensitymap.h"

class ConverterColorSplitNode_Test : public QObject
{
    Q_OBJECT
private slots:
    void colourSplit()
    {
        std::shared_ptr<QtNodes::NodeData> data = std::make_shared<VectorMapData>(VectorMap(1, 1, glm::dvec4(0.00, 1.00, 2.00, 3.00)));

        ConverterColorSplitNode node;
        node.setInData(data, 0);

        IntensityMap out_red = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0))->intensityMap();
        IntensityMap out_green = std::dynamic_pointer_cast<IntensityMapData>(node.outData(1))->intensityMap();
        IntensityMap out_blue = std::dynamic_pointer_cast<IntensityMapData>(node.outData(2))->intensityMap();
        IntensityMap out_alpha = std::dynamic_pointer_cast<IntensityMapData>(node.outData(3))->intensityMap();

        QCOMPARE(out_red.at(0, 0), 0.00);
        QCOMPARE(out_green.at(0, 0), 1.00);
        QCOMPARE(out_blue.at(0, 0), 2.00);
        QCOMPARE(out_alpha.at(0, 0), 3.00);
    };
};