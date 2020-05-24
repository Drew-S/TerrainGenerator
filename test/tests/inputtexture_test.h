#pragma once

#include <QtTest>

#include <QImage>
#include <QColor>

#include "../src/Nodeeditor/Nodes/inputtexture.h"

class InputTextureNode_Test : public QObject
{
    Q_OBJECT
private slots:
    void newTexture()
    {
        InputTextureNode node;
        node.created();
        QTest::mouseClick(node._ui.new_texture, Qt::LeftButton);
        QTest::keyClicks(node._new_texture_ui.resolution, "a", Qt::ControlModifier);
        QTest::keyClicks(node._new_texture_ui.resolution, "32");
        QTest::mouseClick(node._new_texture_ui.ok_button, Qt::LeftButton);

        QCOMPARE(node._new_file_res, 32);
        QVERIFY(node._new_file);
        QVERIFY(node._texture != nullptr);

        QImage image = node._texture->image();

        QCOMPARE(image.width(), 32);
        QCOMPARE(image.height(), 32);

        for (int x = 0; x < 32; x++)
        {
            for (int y = 0; y < 32; y++)
            {
                QColor color = image.pixelColor(x, y);
                QCOMPARE(color, QColor(255, 255, 255, 255));
            }
        }

        QCOMPARE(node._texture->filename(), QString("Texture_1"));
    };

    void loadTexture()
    {
        InputTextureNode node;
        node.created();
        QTest::mouseClick(node._ui.load_texture, Qt::LeftButton);

        QImage image = node._texture->image();

        QCOMPARE(image.width(), 2);
        QCOMPARE(image.height(), 2);

        QColor color = image.pixelColor(0, 0);

        // Green
        QCOMPARE(color, QColor(0, 255, 0, 255));

        color = image.pixelColor(0, 1);

        // White
        QCOMPARE(color, QColor(255, 255, 255, 255));

        color = image.pixelColor(1, 0);

        // Red
        QCOMPARE(color, QColor(255, 0, 0, 255));

        color = image.pixelColor(1, 1);

        // Blue
        QCOMPARE(color, QColor(0, 0, 255, 255));
    };

    void save()
    {
        {
            InputTextureNode node;
            node.created();
            QTest::mouseClick(node._ui.load_texture, Qt::LeftButton);

            QJsonObject data = node.save();

            QVERIFY(!data["image_generated"].toBool());
            QCOMPARE(data["image"], QJsonValue(QDir::cleanPath(QString(PWD) + QString("/assets/textures/test.png"))));
        };
        {
            InputTextureNode node;
            node.created();

            QTest::mouseClick(node._ui.new_texture, Qt::LeftButton);
            QTest::keyClicks(node._new_texture_ui.resolution, "a", Qt::ControlModifier);
            QTest::keyClicks(node._new_texture_ui.resolution, "32");
            QTest::mouseClick(node._new_texture_ui.ok_button, Qt::LeftButton);

            QJsonObject data = node.save();

            QVERIFY(data["image_generated"].toBool());
            // Texture 2, as the newTexture call above creates Texture_1
            QCOMPARE(data["image"], QJsonValue(QString("Texture_2")));
        };
    };
};