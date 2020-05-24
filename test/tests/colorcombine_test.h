#pragma once

#include <QtTest>

#include <nodes/NodeData>

#include <glm/vec4.hpp>

#include "../src/Nodeeditor/Nodes/colorcombine.h"
#include "../src/Nodeeditor/Datatypes/pixmap.h"
#include "../src/Nodeeditor/Datatypes/vectormap.h"
#include "../src/Nodeeditor/Datatypes/intensitymap.h"

class ConverterColorCombineNode_Test : public QObject
{
    Q_OBJECT
private slots:
    void redChanged()
    {
        ConverterColorCombineNode node;
        node.created();

        // TODO: figure out why decimal value testing does not work, it does in a minimal reproduce file
        QTest::keyClicks(node._ui.spin_red, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_red, "0");
        QCOMPARE(node._red_val, 0.00);

        QCOMPARE(node._ui.spin_red->value(), 0.00);
        QCOMPARE(node._shared_ui.spin_red->value(), 0.00);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        glm::dvec4 val = result->vectorMap().at(0, 0);

        QCOMPARE(val, glm::dvec4(0.00, 1.00, 1.00, 1.00));

        QTest::keyClicks(node._shared_ui.spin_red, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_red, "1");
        QCOMPARE(node._red_val, 1.00);

        QCOMPARE(node._ui.spin_red->value(), 1.00);
        QCOMPARE(node._shared_ui.spin_red->value(), 1.00);

        result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        val = result->vectorMap().at(0, 0);

        QCOMPARE(val, glm::dvec4(1.00, 1.00, 1.00, 1.00));
    };

    void greenChanged()
    {
        ConverterColorCombineNode node;
        node.created();

        QTest::keyClicks(node._ui.spin_green, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_green, "0");
        QCOMPARE(node._green_val, 0.00);

        QCOMPARE(node._ui.spin_green->value(), 0.00);
        QCOMPARE(node._shared_ui.spin_green->value(), 0.00);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        glm::dvec4 val = result->vectorMap().at(0, 0);

        QCOMPARE(val, glm::dvec4(1.00, 0.00, 1.00, 1.00));

        QTest::keyClicks(node._shared_ui.spin_green, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_green, "1");
        QCOMPARE(node._green_val, 1.00);

        QCOMPARE(node._ui.spin_green->value(), 1.00);
        QCOMPARE(node._shared_ui.spin_green->value(), 1.00);

        result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        val = result->vectorMap().at(0, 0);

        QCOMPARE(val, glm::dvec4(1.00, 1.00, 1.00, 1.00));
    };

    void blueChanged()
    {
        ConverterColorCombineNode node;
        node.created();

        QTest::keyClicks(node._ui.spin_blue, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_blue, "0");
        QCOMPARE(node._blue_val, 0.00);

        QCOMPARE(node._ui.spin_blue->value(), 0.00);
        QCOMPARE(node._shared_ui.spin_blue->value(), 0.00);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        glm::dvec4 val = result->vectorMap().at(0, 0);

        QCOMPARE(val, glm::dvec4(1.00, 1.00, 0.00, 1.00));

        QTest::keyClicks(node._shared_ui.spin_blue, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_blue, "1");
        QCOMPARE(node._blue_val, 1.00);

        QCOMPARE(node._ui.spin_blue->value(), 1.00);
        QCOMPARE(node._shared_ui.spin_blue->value(), 1.00);

        result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        val = result->vectorMap().at(0, 0);

        QCOMPARE(val, glm::dvec4(1.00, 1.00, 1.00, 1.00));
    };

    void alphaChanged()
    {
        ConverterColorCombineNode node;
        node.created();

        QTest::keyClicks(node._ui.spin_alpha, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_alpha, "0");
        QCOMPARE(node._alpha_val, 0.00);

        QCOMPARE(node._ui.spin_alpha->value(), 0.00);
        QCOMPARE(node._shared_ui.spin_alpha->value(), 0.00);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        glm::dvec4 val = result->vectorMap().at(0, 0);

        QCOMPARE(val, glm::dvec4(1.00, 1.00, 1.00, 0.00));

        QTest::keyClicks(node._shared_ui.spin_alpha, "a", Qt::ControlModifier);
        QTest::keyClicks(node._ui.spin_alpha, "1");
        QCOMPARE(node._alpha_val, 1.00);

        QCOMPARE(node._ui.spin_alpha->value(), 1.00);
        QCOMPARE(node._shared_ui.spin_alpha->value(), 1.00);

        result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        val = result->vectorMap().at(0, 0);

        QCOMPARE(val, glm::dvec4(1.00, 1.00, 1.00, 1.00));
    };

    void inRed()
    {
        ConverterColorCombineNode node;
        node.created();

        std::shared_ptr<IntensityMapData> data = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 0.50));
        node.setInData(data, 0);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        glm::dvec4 val = result->vectorMap().at(0, 0);

        QCOMPARE(val, glm::dvec4(0.50, 1.00, 1.00, 1.00));
    };

    void inGreen()
    {
        ConverterColorCombineNode node;
        node.created();

        std::shared_ptr<IntensityMapData> data = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 0.50));
        node.setInData(data, 1);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        glm::dvec4 val = result->vectorMap().at(0, 0);

        QCOMPARE(val, glm::dvec4(1.00, 0.50, 1.00, 1.00));
    };

    void inBlue()
    {
        ConverterColorCombineNode node;
        node.created();

        std::shared_ptr<IntensityMapData> data = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 0.50));
        node.setInData(data, 2);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        glm::dvec4 val = result->vectorMap().at(0, 0);

        QCOMPARE(val, glm::dvec4(1.00, 1.00, 0.50, 1.00));
    };

    void inAlpha()
    {
        ConverterColorCombineNode node;
        node.created();

        std::shared_ptr<IntensityMapData> data = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 0.50));
        node.setInData(data, 3);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        glm::dvec4 val = result->vectorMap().at(0, 0);

        QCOMPARE(val, glm::dvec4(1.00, 1.00, 1.00, 0.50));
    };

    void multiple()
    {
        ConverterColorCombineNode node;
        node.created();

        std::shared_ptr<IntensityMapData> data_red = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 0.25));
        std::shared_ptr<IntensityMapData> data_green = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 0.50));
        std::shared_ptr<IntensityMapData> data_blue = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 0.75));
        std::shared_ptr<IntensityMapData> data_alpha = std::make_shared<IntensityMapData>(IntensityMap(1, 1, 0.99));
        node.setInData(data_red, 0);
        node.setInData(data_green, 1);
        node.setInData(data_blue, 2);
        node.setInData(data_alpha, 3);

        std::shared_ptr<VectorMapData> result = std::dynamic_pointer_cast<VectorMapData>(node.outData(0));
        glm::dvec4 val = result->vectorMap().at(0, 0);

        QCOMPARE(val, glm::dvec4(0.25, 0.50, 0.75, 0.99));
    };
};