#pragma once

#include <QDir>
#include <QTemporaryDir>

class GlobalData
{
public:
    static GlobalData *getInstance();
    ~GlobalData();
    QString tmpDir();

private:
    GlobalData();
    static bool _instance;
    static GlobalData *_single;

    QTemporaryDir *_tmp_dir;
};