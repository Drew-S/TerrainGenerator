#pragma once

#include <QtTest>

#include "../src/Nodeeditor/Nodes/math.h"

#include "../src/Nodeeditor/Datatypes/intensitymap.h"
#include "../src/Nodeeditor/Datatypes/pixmap.h"

class ConverterMathNode_Test : public QObject
{
    Q_OBJECT;
private slots:
    void modeSwitch()
    {
        ConverterMathNode node;
        node.created();

        QTest::mouseClick(node._ui.mode, Qt::LeftButton);
        QTest::keyClick(node._ui.mode, Qt::Key_Down);
        QTest::keyClick(node._ui.mode, Qt::Key_Enter);

        QCOMPARE(node._mode, ConverterMathNode::ADD);
    };

    void setInValues()
    {
        ConverterMathNode node;
        node.created();

        QTest::keyClicks(node._ui.val_in_0, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.val_in_0, "3");

        QCOMPARE(node._val_in_0, 3.00);
        QCOMPARE(node._ui.val_in_0->value(), 3.00);
        QCOMPARE(node._shared_ui.val_in_0->value(), 3.00);

        QTest::keyClicks(node._ui.val_in_1, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.val_in_1, "5");

        QCOMPARE(node._val_in_1, 5.00);
        QCOMPARE(node._ui.val_in_1->value(), 5.00);
        QCOMPARE(node._shared_ui.val_in_1->value(), 5.00);

        std::shared_ptr<IntensityMapData> result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 4.00);
    };

    void mix()
    {
        ConverterMathNode node;
        node.created();

        std::shared_ptr<IntensityMapData> result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 1.00);

        QTest::keyClicks(node._ui.val_in_0, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.val_in_0, "3");

        QCOMPARE(node._shared_ui.val_in_0->value(), 3.00);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 2.00);

        std::shared_ptr<IntensityMapData> data_a = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 0.00));
        std::shared_ptr<IntensityMapData> data_b = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 2.00));

        node.setInData(data_a, 0);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 0.50);

        node.setInData(data_b, 1);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 1.00);
    };

    void add()
    {
        ConverterMathNode node;
        node.created();

        node._mode = ConverterMathNode::ADD;
        node._generate();

        std::shared_ptr<IntensityMapData> result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 2.00);

        QTest::keyClicks(node._ui.val_in_0, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.val_in_0, "3");

        QCOMPARE(node._shared_ui.val_in_0->value(), 3.00);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 4.00);

        std::shared_ptr<IntensityMapData> data_a = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 0.00));
        std::shared_ptr<IntensityMapData> data_b = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 2.00));

        node.setInData(data_a, 0);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 1.00);

        node.setInData(data_b, 1);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 2.00);
    };

    void subtract()
    {
        ConverterMathNode node;
        node.created();

        node._mode = ConverterMathNode::SUBTRACT;
        node._generate();

        std::shared_ptr<IntensityMapData> result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 0.00);

        QTest::keyClicks(node._ui.val_in_0, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.val_in_0, "3");

        QCOMPARE(node._shared_ui.val_in_0->value(), 3.00);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 2.00);

        std::shared_ptr<IntensityMapData> data_a = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 0.00));
        std::shared_ptr<IntensityMapData> data_b = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 2.00));

        node.setInData(data_a, 0);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), -1.00);

        node.setInData(data_b, 1);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), -2.00);
    };

    void multiply()
    {
        ConverterMathNode node;
        node.created();

        node._mode = ConverterMathNode::MULTIPLY;
        node._generate();

        std::shared_ptr<IntensityMapData> result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 1.00);

        QTest::keyClicks(node._ui.val_in_0, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.val_in_0, "3");

        QCOMPARE(node._shared_ui.val_in_0->value(), 3.00);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 3.00);

        std::shared_ptr<IntensityMapData> data_a = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 0.00));
        std::shared_ptr<IntensityMapData> data_b = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 2.00));

        node.setInData(data_a, 0);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 0.00);

        node.setInData(data_b, 1);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 0.00);
    };

    void divide()
    {
        ConverterMathNode node;
        node.created();

        node._mode = ConverterMathNode::DIVIDE;
        node._generate();

        std::shared_ptr<IntensityMapData> result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 1.00);

        QTest::keyClicks(node._ui.val_in_0, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.val_in_0, "3");

        QCOMPARE(node._shared_ui.val_in_0->value(), 3.00);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 3.00);

        std::shared_ptr<IntensityMapData> data_a = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 1.00));
        std::shared_ptr<IntensityMapData> data_b = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 2.00));

        node.setInData(data_a, 0);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 1.00);

        node.setInData(data_b, 1);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 0.50);
    };

    void min()
    {
        ConverterMathNode node;
        node.created();

        node._mode = ConverterMathNode::MIN;
        node._generate();

        std::shared_ptr<IntensityMapData> result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 1.00);

        QTest::keyClicks(node._ui.val_in_0, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.val_in_0, "3");

        QCOMPARE(node._shared_ui.val_in_0->value(), 3.00);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 1.00);

        std::shared_ptr<IntensityMapData> data_a = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 1.00));
        std::shared_ptr<IntensityMapData> data_b = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 2.00));

        node.setInData(data_a, 0);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 1.00);

        node.setInData(data_b, 1);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 1.00);
    };

    void max()
    {
        ConverterMathNode node;
        node.created();

        node._mode = ConverterMathNode::MAX;
        node._generate();

        std::shared_ptr<IntensityMapData> result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 1.00);

        QTest::keyClicks(node._ui.val_in_0, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.val_in_0, "3");

        QCOMPARE(node._shared_ui.val_in_0->value(), 3.00);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 3.00);

        std::shared_ptr<IntensityMapData> data_a = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 1.00));
        std::shared_ptr<IntensityMapData> data_b = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 2.00));

        node.setInData(data_a, 0);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 1.00);

        node.setInData(data_b, 1);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 2.00);
    };

    void pow()
    {
        ConverterMathNode node;
        node.created();

        node._mode = ConverterMathNode::POW;
        node._generate();

        std::shared_ptr<IntensityMapData> result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 1.00);

        QTest::keyClicks(node._ui.val_in_0, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.val_in_0, "3");

        QCOMPARE(node._shared_ui.val_in_0->value(), 3.00);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 3.00);

        std::shared_ptr<IntensityMapData> data_a = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 2.00));
        std::shared_ptr<IntensityMapData> data_b = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 2.00));

        node.setInData(data_a, 0);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 2.00);

        node.setInData(data_b, 1);

        result = std::dynamic_pointer_cast<IntensityMapData>(node.outData(0));
        QCOMPARE(result->intensityMap().at(0, 0), 4.00);
    };
};