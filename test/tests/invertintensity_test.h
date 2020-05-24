#pragma once

#include <QtTest>

#include <nodes/NodeData>
#include <nodes/NodeDataModel>

#include "../src/Nodeeditor/Nodes/invertintensity.h"

#include "../src/Nodeeditor/Datatypes/pixmap.h"
#include "../src/Nodeeditor/Datatypes/intensitymap.h"

class ConverterInvertIntensityNode_Test : public QObject
{
    Q_OBJECT
private slots:
    void invert()
    {
        for (int i = -9; i < 10; i++)
        {
            ConverterInvertIntensityNode node;
            std::shared_ptr<QtNodes::NodeData> data = std::make_shared<IntensityMapData>(IntensityMap(1, 1, (double)i));
            node.setInData(data, 0);
            QCOMPARE(node._output.at(0, 0), 1.00 - (double)i);

            std::shared_ptr<IntensityMapData> result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
            QCOMPARE(result->intensityMap().at(0, 0), 1.00 - i);
        }
    };
};