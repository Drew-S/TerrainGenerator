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
                QCOMPARE(image.pixelColor(x, y).red(), 255);
                QCOMPARE(image.pixelColor(x, y).green(), 255);
                QCOMPARE(image.pixelColor(x, y).blue(), 255);
                QCOMPARE(image.pixelColor(x, y).alpha(), 255);
            }
        }

        QCOMPARE(node._texture->filename(), QString("Texture_1"));
    };

    void loadTexture()
    {
        InputTextureNode node;
        QTest::mouseClick(node._ui.load_texture, Qt::LeftButton);

        QImage image = node._texture->image();

        QCOMPARE(image.width(), 2);
        QCOMPARE(image.height(), 2);

        // Green
        QCOMPARE(image.pixelColor(0, 0).red(), 0);
        QCOMPARE(image.pixelColor(0, 0).green(), 255);
        QCOMPARE(image.pixelColor(0, 0).blue(), 0);
        QCOMPARE(image.pixelColor(0, 0).alpha(), 255);

        // White
        QCOMPARE(image.pixelColor(0, 1).red(), 255);
        QCOMPARE(image.pixelColor(0, 1).green(), 255);
        QCOMPARE(image.pixelColor(0, 1).blue(), 255);
        QCOMPARE(image.pixelColor(0, 1).alpha(), 255);

        // Red
        QCOMPARE(image.pixelColor(1, 0).red(), 255);
        QCOMPARE(image.pixelColor(1, 0).green(), 0);
        QCOMPARE(image.pixelColor(1, 0).blue(), 0);
        QCOMPARE(image.pixelColor(1, 0).alpha(), 255);

        // Blue
        QCOMPARE(image.pixelColor(1, 1).red(), 0);
        QCOMPARE(image.pixelColor(1, 1).green(), 0);
        QCOMPARE(image.pixelColor(1, 1).blue(), 255);
        QCOMPARE(image.pixelColor(1, 1).alpha(), 255);
    };

    void save()
    {
        {
            InputTextureNode node;
            QTest::mouseClick(node._ui.load_texture, Qt::LeftButton);

            QJsonObject data = node.save();

            QVERIFY(!data["image_generated"].toBool());
            QCOMPARE(data["image"], QJsonValue(QDir::cleanPath(QString(PWD) + QString("/assets/textures/test.png"))));
        };
        {
            InputTextureNode node;

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