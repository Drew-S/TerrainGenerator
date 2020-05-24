#pragma once

#include <QtTest>
#include <QDir>
#include <QSignalSpy>

#include <vector>

#include "../src/Globals/settings.h"

class Settings_Test : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        QVERIFY(SETTINGS);

        this->tmp = SETTINGS->tmpDir().path();
    };

    void renderMode()
    {
        QVERIFY(SETTINGS);

        QVERIFY(!SETTINGS->renderMode());

        SETTINGS->setRenderMode(true);

        QVERIFY(SETTINGS->renderMode());

        SETTINGS->setRenderMode(false);

        QVERIFY(!SETTINGS->renderMode());
    };

    void previewResolution()
    {
        QVERIFY(SETTINGS);

        QCOMPARE(SETTINGS->previewResolution(), 256);

        SETTINGS->setPreviewResolution(512);

        QCOMPARE(SETTINGS->previewResolution(), 512);

        SETTINGS->setPreviewResolution(256);

        QCOMPARE(SETTINGS->previewResolution(), 256);
    };

    void renderResolution()
    {
        QVERIFY(SETTINGS);

        QCOMPARE(SETTINGS->renderResolution(), 1024);

        SETTINGS->setRenderResolution(512);

        QCOMPARE(SETTINGS->renderResolution(), 512);

        SETTINGS->setRenderResolution(1024);

        QCOMPARE(SETTINGS->renderResolution(), 1024);
    };

    void meshResolution()
    {
        QVERIFY(SETTINGS);

        QCOMPARE(SETTINGS->meshResolution(), 256);

        SETTINGS->setMeshResolution(16);

        QCOMPARE(SETTINGS->meshResolution(), 16);

        SETTINGS->setMeshResolution(256);

        QCOMPARE(SETTINGS->meshResolution(), 256);
    };

    void tmpDir()
    {
        QVERIFY(SETTINGS);

        QCOMPARE(this->tmp, SETTINGS->tmpDir().path());
    };

    void getAssetDirectories()
    {
        QVERIFY(SETTINGS);

        std::vector<QDir> dirs = SETTINGS->getAssetDirectories();
        QCOMPARE((int)dirs.size(), 1);

        QCOMPARE(dirs[0].path(), QDir::cleanPath(QString(PWD) + QString("/assets")));
    };

private:
    QString tmp;
};