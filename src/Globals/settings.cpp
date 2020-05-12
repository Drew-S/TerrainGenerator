#include "settings.h"

bool Settings::_instance = false;
Settings *Settings::_single = nullptr;

Settings::Settings()
{
    this->_tmp_dir = new QTemporaryDir(QDir::tempPath() + "/TerrainGenerator_XXXXXX");
}

Settings::~Settings()
{
    _instance = false;
}

Settings *Settings::getInstance()
{
    if (!_instance)
    {
        _instance = true;
        _single = new Settings();
    }
    return _single;
}

QString Settings::tmpDir()
{
    return this->_tmp_dir->path();
}