#pragma once

#include <QtTest>

#include "../src/Nodeeditor/Nodes/constantvector.h"

#include "../src/Nodeeditor/Datatypes/pixmap.h"
#include "../src/Nodeeditor/Datatypes/vectormap.h"

#include <glm/vec4.hpp>

class InputConstantVectorNode_Test : public QObject
{
    Q_OBJECT
private slots:
    void xValue()
    {
        InputConstantVectorNode node;
        node.created();

        QTest::keyClicks(node._ui.spin_x, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_x, "2");
        QCOMPARE(node._x, 2.00);

        QCOMPARE(node._ui.spin_x->value(), 2.00);
        QCOMPARE(node._shared_ui.spin_x->value(), 2.00);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(2.00, 0.00, 0.00, 0.00));

        QTest::keyClicks(node._ui.spin_x, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_x, "1");
        QCOMPARE(node._x, 1.00);

        QCOMPARE(node._ui.spin_x->value(), 1.00);
        QCOMPARE(node._shared_ui.spin_x->value(), 1.00);

        result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(1.00, 0.00, 0.00, 0.00));
    };

    void yValue()
    {
        InputConstantVectorNode node;
        node.created();

        QTest::keyClicks(node._ui.spin_y, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_y, "2");
        QCOMPARE(node._y, 2.00);

        QCOMPARE(node._ui.spin_y->value(), 2.00);
        QCOMPARE(node._shared_ui.spin_y->value(), 2.00);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(0.00, 2.00, 0.00, 0.00));

        QTest::keyClicks(node._ui.spin_y, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_y, "1");
        QCOMPARE(node._y, 1.00);

        QCOMPARE(node._ui.spin_y->value(), 1.00);
        QCOMPARE(node._shared_ui.spin_y->value(), 1.00);

        result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(0.00, 1.00, 0.00, 0.00));
    };

    void zValue()
    {
        InputConstantVectorNode node;
        node.created();

        QTest::keyClicks(node._ui.spin_z, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_z, "2");
        QCOMPARE(node._z, 2.00);

        QCOMPARE(node._ui.spin_z->value(), 2.00);
        QCOMPARE(node._shared_ui.spin_z->value(), 2.00);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(0.00, 0.00, 2.00, 0.00));

        QTest::keyClicks(node._ui.spin_z, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_z, "1");
        QCOMPARE(node._z, 1.00);

        QCOMPARE(node._ui.spin_z->value(), 1.00);
        QCOMPARE(node._shared_ui.spin_z->value(), 1.00);

        result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(0.00, 0.00, 1.00, 0.00));
    };

    void wValue()
    {

        InputConstantVectorNode node;
        node.created();

        QTest::keyClicks(node._ui.spin_w, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_w, "2");
        QCOMPARE(node._w, 2.00);

        QCOMPARE(node._ui.spin_w->value(), 2.00);
        QCOMPARE(node._shared_ui.spin_w->value(), 2.00);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(0.00, 0.00, 0.00, 2.00));

        QTest::keyClicks(node._ui.spin_w, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_w, "1");
        QCOMPARE(node._w, 1.00);

        QCOMPARE(node._ui.spin_w->value(), 1.00);
        QCOMPARE(node._shared_ui.spin_w->value(), 1.00);

        result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(0.00, 0.00, 0.00, 1.00));
    };

    void mixed()
    {
        InputConstantVectorNode node;
        node.created();

        QTest::keyClicks(node._ui.spin_x, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_x, "2");

        QTest::keyClicks(node._ui.spin_y, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_y, "3");

        QTest::keyClicks(node._ui.spin_z, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_z, "4");

        QTest::keyClicks(node._ui.spin_w, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_w, "5");
        QCOMPARE(node._x, 2.00);
        QCOMPARE(node._y, 3.00);
        QCOMPARE(node._z, 4.00);
        QCOMPARE(node._w, 5.00);

        QCOMPARE(node._ui.spin_x->value(), 2.00);
        QCOMPARE(node._shared_ui.spin_x->value(), 2.00);

        QCOMPARE(node._ui.spin_y->value(), 3.00);
        QCOMPARE(node._shared_ui.spin_y->value(), 3.00);

        QCOMPARE(node._ui.spin_z->value(), 4.00);
        QCOMPARE(node._shared_ui.spin_z->value(), 4.00);

        QCOMPARE(node._ui.spin_w->value(), 5.00);
        QCOMPARE(node._shared_ui.spin_w->value(), 5.00);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(2.00, 3.00, 4.00, 5.00));

        QTest::keyClicks(node._shared_ui.spin_x, "a", Qt::ControlModifier);
        QTest::keyClicks(node._shared_ui.spin_x, "6");

        QTest::keyClicks(node._shared_ui.spin_y, "a", Qt::ControlModifier);
        QTest::keyClicks(node._shared_ui.spin_y, "7");

        QTest::keyClicks(node._shared_ui.spin_z, "a", Qt::ControlModifier);
        QTest::keyClicks(node._shared_ui.spin_z, "8");

        QTest::keyClicks(node._shared_ui.spin_w, "a", Qt::ControlModifier);
        QTest::keyClicks(node._shared_ui.spin_w, "9");
        QCOMPARE(node._x, 6.00);
        QCOMPARE(node._y, 7.00);
        QCOMPARE(node._z, 8.00);
        QCOMPARE(node._w, 9.00);

        QCOMPARE(node._ui.spin_x->value(), 6.00);
        QCOMPARE(node._shared_ui.spin_x->value(), 6.00);

        QCOMPARE(node._ui.spin_y->value(), 7.00);
        QCOMPARE(node._shared_ui.spin_y->value(), 7.00);

        QCOMPARE(node._ui.spin_z->value(), 8.00);
        QCOMPARE(node._shared_ui.spin_z->value(), 8.00);

        QCOMPARE(node._ui.spin_w->value(), 9.00);
        QCOMPARE(node._shared_ui.spin_w->value(), 9.00);

        result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(6.00, 7.00, 8.00, 9.00));
    };
};