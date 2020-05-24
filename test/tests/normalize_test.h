#pragma once

#include <math.h>

#include <QtTest>

#include <nodes/NodeData>
#include <nodes/NodeDataModel>

#include "../src/Nodeeditor/Nodes/normalize.h"

#include "../src/Nodeeditor/Datatypes/pixmap.h"
#include "../src/Nodeeditor/Datatypes/vectormap.h"

#include <glm/vec4.hpp>

#define NORM(v) v / (sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w)))

static const glm::dvec4 v{1.00, 2.00, 3.00, 4.00};

static const glm::dvec4 r = NORM(v);

class ConverterNormalizeNode_Test : public QObject
{
    Q_OBJECT
private slots:
    void normalize()
    {
        ConverterNormalizeNode node;
        std::shared_ptr<QtNodes::NodeData> data = std::make_shared<VectorMapData>(VectorMap(1, 1, v));
        node.setInData(data, 0);
        QCOMPARE(node._output.at(0, 0), r);
        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        QCOMPARE(result->vectorMap().at(0, 0), r);
    };
};