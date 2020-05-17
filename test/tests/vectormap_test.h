#pragma once

#include <vector>

#include <QtTest>

#include <QImage>
#include <QPixmap>
#include <QColor>

#include "../../src/Nodeeditor/Datatypes/vectormap.h"
#include "../../src/Nodeeditor/Datatypes/intensitymap.h"

#include <glm/vec4.hpp>

#define ZERO glm::dvec4(0.00, 0.00, 0.00, 0.00)
#define VEC_1000 glm::dvec4(1.00, 0.00, 0.00, 0.00)
#define VEC_0100 glm::dvec4(0.00, 1.00, 0.00, 0.00)
#define VEC_0010 glm::dvec4(0.00, 0.00, 1.00, 0.00)
#define VEC_0001 glm::dvec4(0.00, 0.00, 0.00, 1.00)

class VectorMap_Test : public QObject
{
    Q_OBJECT
private slots:
    void constructed()
    {
        VectorMap map(10, 20);
        QCOMPARE(map.width, 10);
        QCOMPARE(map.height, 20);
        for (int y = 0; y < map.height; y++)
            for (int x = 0; x < map.width; x++)
                QCOMPARE(map.at(x, y), ZERO);

        std::vector<glm::dvec4> values{VEC_1000, VEC_0100, VEC_0010, VEC_0001};

        map = VectorMap(4, 1, values);
        QCOMPARE(map.width, 4);
        QCOMPARE(map.height, 1);
        QCOMPARE(map.at(-1, 0), ZERO);
        QCOMPARE(map.at(0, -1), ZERO);
        QCOMPARE(map.at(4, 0), ZERO);
        QCOMPARE(map.at(0, 1), ZERO);
        QCOMPARE(map.at(0, 0), VEC_1000);
        QCOMPARE(map.at(3, 0), VEC_0001);
    };

    void constructedImage()
    {
        QImage image(2, 2, QImage::Format_ARGB32);
        image.fill(QColor(255, 0, 0, 0));

        VectorMap map(image);
        QCOMPARE(map.at(0, 0), VEC_1000);
        QCOMPARE(map.at(1, 0), VEC_1000);
        QCOMPARE(map.at(0, 1), VEC_1000);
        QCOMPARE(map.at(1, 1), VEC_1000);

        image.fill(QColor(0, 255, 0, 0));

        map = VectorMap(image);
        QCOMPARE(map.at(0, 0), VEC_0100);
        QCOMPARE(map.at(1, 0), VEC_0100);
        QCOMPARE(map.at(0, 1), VEC_0100);
        QCOMPARE(map.at(1, 1), VEC_0100);
    };

    void constructedPixmap()
    {
        QImage image(2, 2, QImage::Format_ARGB32);
        image.fill(QColor(0, 0, 255, 0));
        QPixmap pixmap = QPixmap::fromImage(image, Qt::NoFormatConversion);

        VectorMap map(pixmap);
        QCOMPARE(map.at(0, 0), VEC_0010);
        QCOMPARE(map.at(1, 0), VEC_0010);
        QCOMPARE(map.at(0, 1), VEC_0010);
        QCOMPARE(map.at(1, 1), VEC_0010);
    };

    void at()
    {

        std::vector<glm::dvec4> values{VEC_1000, VEC_0100, VEC_0010, VEC_0001};
        VectorMap map(2, 2, values);

        QCOMPARE(map.at(-1, 0), ZERO);
        QCOMPARE(map.at(0, -1), ZERO);
        QCOMPARE(map.at(2, 0), ZERO);
        QCOMPARE(map.at(0, 2), ZERO);
        QCOMPARE(map.at(-1, -1), ZERO);
        QCOMPARE(map.at(2, 2), ZERO);

        QCOMPARE(map.at(0, 0), VEC_1000);
        QCOMPARE(map.at(1, 0), VEC_0100);
        QCOMPARE(map.at(0, 1), VEC_0010);
        QCOMPARE(map.at(1, 1), VEC_0001);
    };

    void append()
    {
        VectorMap map(2, 2);
        for (int y = -1; y < 3; y++)
            for (int x = -1; x < 3; x++)
                QCOMPARE(map.at(x, y), ZERO);

        qDebug("Here?");
        QVERIFY(map.append(VEC_1000));
        QVERIFY(map.append(VEC_0100));
        QVERIFY(map.append(VEC_0010));
        QVERIFY(map.append(VEC_0001));

        QCOMPARE(map.at(0, 0), VEC_1000);
        QCOMPARE(map.at(1, 0), VEC_0100);
        QCOMPARE(map.at(0, 1), VEC_0010);
        QCOMPARE(map.at(1, 1), VEC_0001);
    };

    void set()
    {
        std::vector<glm::dvec4> values{VEC_1000, VEC_1000, VEC_1000, VEC_1000};
        VectorMap map(2, 2, values);
        for (int y = 0; y < 2; y++)
            for (int x = 0; x < 2; x++)
                QCOMPARE(map.at(x, y), VEC_1000);

        map.set(0, 0, ZERO);
        map.set(1, 0, VEC_0100);
        map.set(0, 1, VEC_0010);
        map.set(1, 1, VEC_0001);

        QCOMPARE(map.at(0, 0), ZERO);
        QCOMPARE(map.at(1, 0), VEC_0100);
        QCOMPARE(map.at(0, 1), VEC_0010);
        QCOMPARE(map.at(1, 1), VEC_0001);
    };

    void toIntensityMap()
    {
        std::vector<glm::dvec4> data{glm::dvec4(1.00, 0.50, 0.25, 0.125)};
        VectorMap from(1, 1, data);

        IntensityMap to = from.toIntensityMap();
        QCOMPARE(to.at(0, 0), 0.25);
    };

    void fromIntensityMap()
    {
        std::vector<double> data{0.50};
        IntensityMap from(1, 1, data);

        VectorMap to = VectorMap::fromIntensityMap(from, glm::dvec4(1.00, 1.00, 1.00, 2.00), VectorMap::APPLY);
        QCOMPARE(to.at(0, 0), glm::dvec4(0.50, 0.50, 0.50, 1.00));

        to = VectorMap::fromIntensityMap(from, glm::dvec4(1.00, 1.00, 1.00, 2.00), VectorMap::OVERRIDE_COLOR);
        QCOMPARE(to.at(0, 0), glm::dvec4(0.50, 0.50, 0.50, 2.00));

        to = VectorMap::fromIntensityMap(from, glm::dvec4(1.00, 1.00, 1.00, 2.00), VectorMap::OVERRIDE_MAP);
        QCOMPARE(to.at(0, 0), glm::dvec4(0.50, 0.50, 0.50, 0.50));

        to = VectorMap::fromIntensityMap(from, glm::dvec4(1.00, 1.00, 1.00, 2.00), VectorMap::MASK);
        QCOMPARE(to.at(0, 0), glm::dvec4(1.00, 1.00, 1.00, 0.50));

        to = VectorMap::fromIntensityMap(from, glm::dvec4(1.00, 1.00, 1.00, 2.00), VectorMap::MASK_ALPHA);
        QCOMPARE(to.at(0, 0), glm::dvec4(1.00, 1.00, 1.00, 1.00));
    };
};
