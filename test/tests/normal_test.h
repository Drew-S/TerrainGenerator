#pragma once

#include <vector>

#include <QtTest>
#include <QImage>

#include "../src/Nodeeditor/Nodes/Normal/normal.h"

#include "../src/Nodeeditor/Datatypes/intensitymap.h"

class NormalMapGenerator_Test : public QObject
{
    Q_OBJECT
private slots:
    void constructor()
    {
        // 1 0
        // 0 1
        std::vector<double> data{1.00, 0.00, 0.00, 1.00};
        IntensityMap map(2, 2, data);

        NormalMapGenerator normal(&map);
        normal.generate();

        QImage image = normal.toImage();
        QColor color = image.pixelColor(0, 0);
        QCOMPARE(color.red(), 0);
        QCOMPARE(color.green(), 0);
        QCOMPARE(color.blue(), 187);
        QCOMPARE(color.alpha(), 255);

        color = image.pixelColor(0, 1);
        QCOMPARE(color.red(), 0);
        QCOMPARE(color.green(), 129);
        QCOMPARE(color.blue(), 182);
        QCOMPARE(color.alpha(), 255);

        color = image.pixelColor(1, 0);
        QCOMPARE(color.red(), 129);
        QCOMPARE(color.green(), 0);
        QCOMPARE(color.blue(), 182);
        QCOMPARE(color.alpha(), 255);

        color = image.pixelColor(1, 1);
        QCOMPARE(color.red(), 128);
        QCOMPARE(color.green(), 128);
        QCOMPARE(color.blue(), 179);
        QCOMPARE(color.alpha(), 255);
    };

    // void benchmark()
    // {
    //     std::vector<double> data;
    //     for (int i = 0; i < 2048 * 2048; i++)
    //         data.push_back(i / (2048 * 2048));
    //     IntensityMap map(2048, 2048, data);

    //     NormalMapGenerator normal(&map);

    //     QBENCHMARK
    //     {
    //         normal.generate();
    //     }
    // };
};