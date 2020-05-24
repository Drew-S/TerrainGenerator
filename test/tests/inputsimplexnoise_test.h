#pragma once

#include <QtTest>
#include <QJsonObject>
#include <QJsonValue>
#include <QVector3D>

#include <nodes/NodeDataModel>

#include "../src/Nodeeditor/Nodes/inputsimplexnoise.h"

class InputSimplexNoiseNode_Test : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        this->node.created();
    };

    void octivesChanged()
    {
        QTest::keyClicks(this->node._ui.spin_octives, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._ui.spin_octives, "25");
        QCOMPARE(this->node._octives, 25.0f);

        QCOMPARE(this->node._ui.spin_octives->value(), 25.00);
        QCOMPARE(this->node._shared_ui.spin_octives->value(), 25.00);

        QTest::keyClicks(this->node._shared_ui.spin_octives, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._shared_ui.spin_octives, "12");
        QCOMPARE(this->node._octives, 12.0f);

        QCOMPARE(this->node._ui.spin_octives->value(), 12.00);
        QCOMPARE(this->node._shared_ui.spin_octives->value(), 12.00);
    };

    void frequencyChanged()
    {
        QTest::keyClicks(this->node._ui.spin_frequency, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._ui.spin_frequency, "10");
        QCOMPARE(this->node._frequency, 10.0f);

        QCOMPARE(this->node._ui.spin_frequency->value(), 10.00);
        QCOMPARE(this->node._shared_ui.spin_frequency->value(), 10.00);

        QTest::keyClicks(this->node._shared_ui.spin_frequency, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._shared_ui.spin_frequency, "5");
        QCOMPARE(this->node._frequency, 5.0f);

        QCOMPARE(this->node._ui.spin_frequency->value(), 5.00);
        QCOMPARE(this->node._shared_ui.spin_frequency->value(), 5.00);
    };

    void persistenceChanged()
    {
        QTest::keyClicks(this->node._ui.spin_persistence, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._ui.spin_persistence, "2");
        QCOMPARE(this->node._persistence, 2.0f);

        QCOMPARE(this->node._ui.spin_persistence->value(), 2.00);
        QCOMPARE(this->node._shared_ui.spin_persistence->value(), 2.00);

        QTest::keyClicks(this->node._shared_ui.spin_persistence, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._shared_ui.spin_persistence, "1");
        QCOMPARE(this->node._persistence, 1.0f);

        QCOMPARE(this->node._ui.spin_persistence->value(), 1.00);
        QCOMPARE(this->node._shared_ui.spin_persistence->value(), 1.00);
    };

    void xChanged()
    {
        QTest::keyClicks(this->node._ui.spin_x, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._ui.spin_x, "3");
        QCOMPARE(this->node._offset.x(), 3.0f);

        QCOMPARE(this->node._ui.spin_x->value(), 3.00);
        QCOMPARE(this->node._shared_ui.spin_x->value(), 3.00);

        QTest::keyClicks(this->node._shared_ui.spin_x, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._shared_ui.spin_x, "1");
        QCOMPARE(this->node._offset.x(), 1.0f);

        QCOMPARE(this->node._ui.spin_x->value(), 1.00);
        QCOMPARE(this->node._shared_ui.spin_x->value(), 1.00);
    };

    void yChanged()
    {
        QTest::keyClicks(this->node._ui.spin_y, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._ui.spin_y, "4");
        QCOMPARE(this->node._offset.y(), 4.0f);

        QCOMPARE(this->node._ui.spin_y->value(), 4.00);
        QCOMPARE(this->node._shared_ui.spin_y->value(), 4.00);

        QTest::keyClicks(this->node._shared_ui.spin_y, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._shared_ui.spin_y, "2");
        QCOMPARE(this->node._offset.y(), 2.0f);

        QCOMPARE(this->node._ui.spin_y->value(), 2.00);
        QCOMPARE(this->node._shared_ui.spin_y->value(), 2.00);
    };

    void zChanged()
    {
        QTest::keyClicks(this->node._ui.spin_z, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._ui.spin_z, "10");
        QCOMPARE(this->node._offset.z(), 10.0f);

        QCOMPARE(this->node._ui.spin_z->value(), 10.00);
        QCOMPARE(this->node._shared_ui.spin_z->value(), 10.00);

        QTest::keyClicks(this->node._shared_ui.spin_z, "a", Qt::ControlModifier);
        QTest::keyClicks(this->node._shared_ui.spin_z, "5");
        QCOMPARE(this->node._offset.z(), 5.0f);

        QCOMPARE(this->node._ui.spin_z->value(), 5.00);
        QCOMPARE(this->node._shared_ui.spin_z->value(), 5.00);
    };

    void save()
    {
        this->node.octivesChanged(2.00);
        this->node.frequencyChanged(3.00);
        this->node.persistenceChanged(4.00);
        this->node.xChanged(5.00);
        this->node.yChanged(6.00);
        this->node.zChanged(7.00);

        QJsonObject data = this->node.save();

        QCOMPARE(data["octives"], QJsonValue(2.0f));
        QCOMPARE(data["frequency"], QJsonValue(3.0f));
        QCOMPARE(data["persistence"], QJsonValue(4.0f));
        QCOMPARE(data["offset"].toObject()["x"], QJsonValue(5.0f));
        QCOMPARE(data["offset"].toObject()["y"], QJsonValue(6.0f));
        QCOMPARE(data["offset"].toObject()["z"], QJsonValue(7.0f));
    };

    void restore()
    {
        QJsonObject data;
        data["octives"] = 0.5f;
        data["frequency"] = 0.5f;
        data["persistence"] = 0.5f;

        QJsonObject offset;
        offset["x"] = 0.5f;
        offset["y"] = 0.5f;
        offset["z"] = 0.5f;

        data["offset"] = offset;

        this->node.restore(data);

        QCOMPARE(this->node._octives, 0.5f);
        QCOMPARE(this->node._frequency, 0.5f);
        QCOMPARE(this->node._persistence, 0.5f);
        QCOMPARE(this->node._offset, QVector3D(0.5f, 0.5f, 0.5f));

        QCOMPARE(this->node._ui.spin_frequency->value(), 0.50);
        QCOMPARE(this->node._shared_ui.spin_frequency->value(), 0.50);

        QCOMPARE(this->node._ui.spin_octives->value(), 0.50);
        QCOMPARE(this->node._shared_ui.spin_octives->value(), 0.50);

        QCOMPARE(this->node._ui.spin_persistence->value(), 0.50);
        QCOMPARE(this->node._shared_ui.spin_persistence->value(), 0.50);

        QCOMPARE(this->node._ui.spin_x->value(), 0.50);
        QCOMPARE(this->node._shared_ui.spin_x->value(), 0.50);

        QCOMPARE(this->node._ui.spin_y->value(), 0.50);
        QCOMPARE(this->node._shared_ui.spin_y->value(), 0.50);

        QCOMPARE(this->node._ui.spin_z->value(), 0.50);
        QCOMPARE(this->node._shared_ui.spin_z->value(), 0.50);
    };

private:
    InputSimplexNoiseNode node;
};
