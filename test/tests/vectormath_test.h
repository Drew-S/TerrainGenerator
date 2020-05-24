#pragma once

#include <QtTest>

#include "../src/Nodeeditor/Nodes/vectormath.h"

#include "../src/Nodeeditor/Datatypes/vectormap.h"
#include "../src/Nodeeditor/Datatypes/pixmap.h"

#include <glm/vec4.hpp>

#define A glm::dvec4(1.00, 2.00, 3.00, 4.00)
#define B glm::dvec4(5.00, 6.00, 7.00, 8.00)

class ConverterVectorMathNode_Test : public QObject
{
    Q_OBJECT;
private slots:
    void modeSwitch()
    {
        ConverterVectorMathNode node;
        node.created();

        QTest::mouseClick(node._ui.mode, Qt::LeftButton);
        QTest::keyClick(node._ui.mode, Qt::Key_Down);
        QTest::keyClick(node._ui.mode, Qt::Key_Enter);

        QCOMPARE(node._mode, ConverterVectorMathNode::ADD);
    };

    void setInValues()
    {
        ConverterVectorMathNode node;
        node.created();

        QTest::keyClicks(node._ui.spin_x_0, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_x_0, "3");

        QCOMPARE(node._val_in_0.x, 3.00);
        QCOMPARE(node._ui.spin_x_0->value(), 3.00);
        QCOMPARE(node._shared_ui.spin_x_0->value(), 3.00);

        QTest::keyClicks(node._ui.spin_x_1, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_x_1, "5");

        QCOMPARE(node._val_in_1.x, 5.00);
        QCOMPARE(node._ui.spin_x_1->value(), 5.00);
        QCOMPARE(node._shared_ui.spin_x_1->value(), 5.00);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(4.00, 1.00, 1.00, 1.00));
    };

    void mix()
    {
        ConverterVectorMathNode node;
        node.created();

        std::shared_ptr<VectorMapData> data_a = std::make_shared<VectorMapData>(VectorMap(1, 1, A));
        std::shared_ptr<VectorMapData> data_b = std::make_shared<VectorMapData>(VectorMap(1, 1, B));

        node.setInData(data_a, 0);
        node.setInData(data_b, 1);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));

        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(3.00, 4.00, 5.00, 6.00));
    };

    void add()
    {
        ConverterVectorMathNode node;
        node.created();

        node._mode = ConverterVectorMathNode::ADD;
        node._generate();

        std::shared_ptr<VectorMapData> data_a = std::make_shared<VectorMapData>(VectorMap(1, 1, A));
        std::shared_ptr<VectorMapData> data_b = std::make_shared<VectorMapData>(VectorMap(1, 1, B));

        node.setInData(data_a, 0);
        node.setInData(data_b, 1);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));

        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(6.00, 8.00, 10.00, 12.00));
    };

    void subtract()
    {
        ConverterVectorMathNode node;
        node.created();

        node._mode = ConverterVectorMathNode::SUBTRACT;
        node._generate();

        std::shared_ptr<VectorMapData> data_a = std::make_shared<VectorMapData>(VectorMap(1, 1, A));
        std::shared_ptr<VectorMapData> data_b = std::make_shared<VectorMapData>(VectorMap(1, 1, B));

        node.setInData(data_a, 0);
        node.setInData(data_b, 1);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));

        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(-4.00, -4.00, -4.00, -4.00));
    };

    void multiply()
    {
        ConverterVectorMathNode node;
        node.created();

        node._mode = ConverterVectorMathNode::MULTIPLY;
        node._generate();

        std::shared_ptr<VectorMapData> data_a = std::make_shared<VectorMapData>(VectorMap(1, 1, A));
        std::shared_ptr<VectorMapData> data_b = std::make_shared<VectorMapData>(VectorMap(1, 1, B));

        node.setInData(data_a, 0);
        node.setInData(data_b, 1);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));

        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(5.00, 12.00, 21.00, 32.00));
    };

    void divide()
    {
        ConverterVectorMathNode node;
        node.created();

        node._mode = ConverterVectorMathNode::DIVIDE;
        node._generate();

        std::shared_ptr<VectorMapData> data_a = std::make_shared<VectorMapData>(VectorMap(1, 1, A));
        std::shared_ptr<VectorMapData> data_b = std::make_shared<VectorMapData>(VectorMap(1, 1, B));

        node.setInData(data_a, 0);
        node.setInData(data_b, 1);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));

        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(0.200, 0.33333333333333333333, 0.42857142857142857143, 0.5));
    };

    void cross()
    {
        ConverterVectorMathNode node;
        node.created();

        node._mode = ConverterVectorMathNode::CROSS;
        node._generate();

        std::shared_ptr<VectorMapData> data_a = std::make_shared<VectorMapData>(VectorMap(1, 1, A));
        std::shared_ptr<VectorMapData> data_b = std::make_shared<VectorMapData>(VectorMap(1, 1, B));

        node.setInData(data_a, 0);
        node.setInData(data_b, 1);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));

        QCOMPARE(result->vectorMap().at(0, 0), glm::dvec4(-4.00, 8.00, -4.00, 1.00));
    };
};