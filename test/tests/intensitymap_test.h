#pragma once

#include <vector>

#include <QtTest>

#include <QImage>
#include <QPixmap>
#include <QColor>

#include "../../src/Nodeeditor/Datatypes/intensitymap.h"

class IntensityMap_Test : public QObject
{
    Q_OBJECT;
private slots:

    void constructed()
    {
        IntensityMap map(10, 20);
        QCOMPARE(map.width, 10);
        QCOMPARE(map.height, 20);
        for (int y = 0; y < map.height; y++)
            for (int x = 0; x < map.width; x++)
                QCOMPARE(map.at(x, y), 0.00);

        std::vector<double> values{1.00, 2.00, 3.00, 4.00};
        map = IntensityMap(4, 1, values);
        QCOMPARE(map.width, 4);
        QCOMPARE(map.height, 1);
        QCOMPARE(map.at(-1, 0), 0.00);
        QCOMPARE(map.at(0, -1), 0.00);
        QCOMPARE(map.at(4, 0), 0.00);
        QCOMPARE(map.at(0, 1), 0.00);
        QCOMPARE(map.at(0, 0), 1.00);
        QCOMPARE(map.at(3, 0), 4.00);
    };

    void constructedImage()
    {
        QImage image(2, 2, QImage::Format_RGB32);
        image.fill(QColor(255, 100, 0, 255));

        double average = (255.00 * 2.00 + 100.00) / (4.00 * 255.00);
        double green = 100.00 / 255.00;

        IntensityMap map(image, IntensityMap::RED);
        QCOMPARE(map.at(0, 0), 1.00);
        QCOMPARE(map.at(1, 0), 1.00);
        QCOMPARE(map.at(0, 1), 1.00);
        QCOMPARE(map.at(1, 1), 1.00);

        map = IntensityMap(image, IntensityMap::BLUE);
        QCOMPARE(map.at(0, 0), 0.00);
        QCOMPARE(map.at(1, 0), 0.00);
        QCOMPARE(map.at(0, 1), 0.00);
        QCOMPARE(map.at(1, 1), 0.00);

        map = IntensityMap(image, IntensityMap::GREEN);
        QCOMPARE(map.at(0, 0), green);
        QCOMPARE(map.at(1, 0), green);
        QCOMPARE(map.at(0, 1), green);
        QCOMPARE(map.at(1, 1), green);

        map = IntensityMap(image, IntensityMap::AVERAGE);
        QCOMPARE(map.at(0, 0), average);
        QCOMPARE(map.at(1, 0), average);
        QCOMPARE(map.at(0, 1), average);
        QCOMPARE(map.at(1, 1), average);

        map = IntensityMap(image, IntensityMap::MIN);
        QCOMPARE(map.at(0, 0), 0.00);
        QCOMPARE(map.at(1, 0), 0.00);
        QCOMPARE(map.at(0, 1), 0.00);
        QCOMPARE(map.at(1, 1), 0.00);

        map = IntensityMap(image, IntensityMap::MAX);
        QCOMPARE(map.at(0, 0), 1.00);
        QCOMPARE(map.at(1, 0), 1.00);
        QCOMPARE(map.at(0, 1), 1.00);
        QCOMPARE(map.at(1, 1), 1.00);
    };

    void constructedPixmap()
    {
        QImage image(2, 2, QImage::Format_RGB32);
        image.fill(QColor(255, 100, 0, 255));
        QPixmap pixmap = QPixmap::fromImage(image, Qt::NoFormatConversion);

        IntensityMap map(pixmap, IntensityMap::RED);
        QCOMPARE(map.at(0, 0), 1.00);
        QCOMPARE(map.at(1, 0), 1.00);
        QCOMPARE(map.at(0, 1), 1.00);
        QCOMPARE(map.at(1, 1), 1.00);
    };

    void at()
    {
        std::vector<double> values{1.00, 2.00, 3.00, 4.00};
        IntensityMap map(2, 2, values);

        QCOMPARE(map.at(-1, 0), 0.00);
        QCOMPARE(map.at(0, -1), 0.00);
        QCOMPARE(map.at(2, 0), 0.00);
        QCOMPARE(map.at(0, 2), 0.00);
        QCOMPARE(map.at(-1, -1), 0.00);
        QCOMPARE(map.at(2, 2), 0.00);

        QCOMPARE(map.at(0, 0), 1.00);
        QCOMPARE(map.at(1, 0), 2.00);
        QCOMPARE(map.at(0, 1), 3.00);
        QCOMPARE(map.at(1, 1), 4.00);
    };

    void append()
    {
        IntensityMap map(2, 2);
        for (int y = -1; y < 3; y++)
            for (int x = -1; x < 3; x++)
                QCOMPARE(map.at(x, y), 0.00);

        qDebug("Here?");
        QVERIFY(map.append(1.00));
        QVERIFY(map.append(2.00));
        QVERIFY(map.append(3.00));
        QVERIFY(map.append(4.00));

        QCOMPARE(map.at(0, 0), 1.00);
        QCOMPARE(map.at(1, 0), 2.00);
        QCOMPARE(map.at(0, 1), 3.00);
        QCOMPARE(map.at(1, 1), 4.00);
    };

    void set()
    {
        std::vector<double> values{1.00, 1.00, 1.00, 1.00};
        IntensityMap map(2, 2, values);
        for (int y = 0; y < 2; y++)
            for (int x = 0; x < 2; x++)
                QCOMPARE(map.at(x, y), 1.00);

        for (int y = 0; y < 2; y++)
            for (int x = 0; x < 2; x++)
                map.set(x, y, 2.00 * (double)y + (double)x);

        QCOMPARE(map.at(0, 0), 0.00);
        QCOMPARE(map.at(1, 0), 1.00);
        QCOMPARE(map.at(0, 1), 2.00);
        QCOMPARE(map.at(1, 1), 3.00);
    };
};
