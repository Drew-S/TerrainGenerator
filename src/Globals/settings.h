#pragma once

#include <QDir>
#include <QTemporaryDir>
#include <QObject>

// Global singleton for sharing settings throughout
class Settings : public QObject
{
    Q_OBJECT
public:
    // Get/create a reference to the settings instance
    static Settings *getInstance();
    ~Settings();

    // Getters for settings
    QString tmpDir();

private:
    Settings();
    static bool _instance;
    static Settings *_single;

    // Settings
    QTemporaryDir *_tmp_dir;
};

#define SETTINGS Settings::getInstance()
