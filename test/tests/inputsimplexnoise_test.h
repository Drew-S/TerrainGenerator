#pragma once

#include <QtTest>

#include "../src/Nodeeditor/Nodes/inputsimplexnoise.h"

class InputSimplexNoiseNode_Test : public QObject
{
    Q_OBJECT
private slots:
    void octivesChanged()
    {
        QTest::keyClicks(this->node._ui.spin_octives, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._ui.spin_octives, "25");
        QCOMPARE(this->node._octives, 25.0f);

        QTest::keyClicks(this->node._shared_ui.spin_octives, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._shared_ui.spin_octives, "12");
        QCOMPARE(this->node._octives, 12.0f);
    };

    void frequencyChanged()
    {
        QTest::keyClicks(this->node._ui.spin_frequency, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._ui.spin_frequency, "10");
        QCOMPARE(this->node._frequency, 10.0f);

        QTest::keyClicks(this->node._shared_ui.spin_frequency, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._shared_ui.spin_frequency, "5");
        QCOMPARE(this->node._frequency, 5.0f);
    };

    void persistenceChanged()
    {
        QTest::keyClicks(this->node._ui.spin_persistence, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._ui.spin_persistence, "2");
        QCOMPARE(this->node._persistence, 2.0f);

        QTest::keyClicks(this->node._shared_ui.spin_persistence, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._shared_ui.spin_persistence, "1");
        QCOMPARE(this->node._persistence, 1.0f);
    };

    void xChanged()
    {
        QTest::keyClicks(this->node._ui.spin_x, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._ui.spin_x, "3");
        QCOMPARE(this->node._offset.x(), 3.0f);

        QTest::keyClicks(this->node._shared_ui.spin_x, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._shared_ui.spin_x, "1");
        QCOMPARE(this->node._offset.x(), 1.0f);
    };

    void yChanged()
    {
        QTest::keyClicks(this->node._ui.spin_y, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._ui.spin_y, "4");
        QCOMPARE(this->node._offset.y(), 4.0f);

        QTest::keyClicks(this->node._shared_ui.spin_y, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._shared_ui.spin_y, "2");
        QCOMPARE(this->node._offset.y(), 2.0f);
    };

    void zChanged()
    {
        QTest::keyClicks(this->node._ui.spin_z, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._ui.spin_z, "10");
        QCOMPARE(this->node._offset.z(), 10.0f);

        QTest::keyClicks(this->node._shared_ui.spin_z, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._shared_ui.spin_z, "5");
        QCOMPARE(this->node._offset.z(), 5.0f);
    };

private:
    InputSimplexNoiseNode node;
};