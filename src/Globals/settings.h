#pragma once

#include <QDir>
#include <QTemporaryDir>

class Settings
{
public:
    static Settings *getInstance();
    ~Settings();
    QString tmpDir();

private:
    Settings();
    static bool _instance;
    static Settings *_single;

    QTemporaryDir *_tmp_dir;
};

#define SETTINGS Settings::getInstance()