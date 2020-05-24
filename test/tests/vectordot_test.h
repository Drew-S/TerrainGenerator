#pragma once

#include <QtTest>

#include <nodes/NodeData>
#include <nodes/NodeDataModel>

#include "../src/Nodeeditor/Nodes/vectordot.h"

#include "../src/Nodeeditor/Datatypes/pixmap.h"
#include "../src/Nodeeditor/Datatypes/vectormap.h"
#include "../src/Nodeeditor/Datatypes/intensitymap.h"

#include <glm/vec4.hpp>

static const glm::dvec4 a{2.00, 3.00, 4.00, 5.00};
static const glm::dvec4 b{6.00, 2.00, -3.00, -6.00};

class ConverterVectorDotNode_Test : public QObject
{
    Q_OBJECT
private slots:
    void dot()
    {
        ConverterVectorDotNode node;

        std::shared_ptr<QtNodes::NodeData> data_a = std::make_shared<VectorMapData>(VectorMap(1, 1, a));
        node.setInData(data_a, 0);

        std::shared_ptr<IntensityMapData> result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 14.00);

        std::shared_ptr<QtNodes::NodeData> data_b = std::make_shared<VectorMapData>(VectorMap(1, 1, b));
        node.setInData(data_b, 1);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), -24.00);
    };
};