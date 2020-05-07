#pragma once

#include <vector>

#include <QtTest>

#include <QImage>
#include <QPixmap>
#include <QColor>

#include "../src/Nodeeditor/Datatypes/vectormap.h"

class VectorMap_Test : public QObject
{
    Q_OBJECT
private slots:
    void constructed(){};
    void constructedImage(){};
    void constructedPixmap(){};

    void at(){};
    void append(){};
    void set(){};
};