#include "settings.h"

// Setup data for singleton
bool Settings::_instance = false;
Settings *Settings::_single = nullptr;

// Create settings and setup default data
Settings::Settings()
{
    this->_tmp_dir = new QTemporaryDir(QDir::tempPath() + "/TerrainGenerator_XXXXXX");
}

Settings::~Settings()
{
    _instance = false;
}

// Get/create a reference to the settings instance
Settings *Settings::getInstance()
{
    if (!_instance)
    {
        _instance = true;
        _single = new Settings();
    }
    return _single;
}

// Get the temp directory
QString Settings::tmpDir()
{
    return this->_tmp_dir->path();
}
