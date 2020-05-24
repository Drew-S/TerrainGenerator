#pragma once

#include <QtTest>

#include "../src/Nodeeditor/Nodes/constantvalue.h"

#include "../src/Nodeeditor/Datatypes/pixmap.h"
#include "../src/Nodeeditor/Datatypes/intensitymap.h"

class InputConstantValueNode_Test : public QObject
{
    Q_OBJECT

private slots:
    void valueChanged()
    {
        InputConstantValueNode node;
        node.created();

        QTest::keyClicks(node._ui.constant, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.constant, "2");
        QCOMPARE(node._value, 2.00);

        QCOMPARE(node._ui.constant->value(), 2.00);
        QCOMPARE(node._shared_ui.constant->value(), 2.00);

        std::shared_ptr<IntensityMapData> result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 2.00);

        QTest::keyClicks(node._shared_ui.constant, "a", Qt::ControlModifier);
        QTest::keyClicks(node._shared_ui.constant, "5");
        QCOMPARE(node._value, 5.00);

        QCOMPARE(node._ui.constant->value(), 5.00);
        QCOMPARE(node._shared_ui.constant->value(), 5.00);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 5.00);
    };
};