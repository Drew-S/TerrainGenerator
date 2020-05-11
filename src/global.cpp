#include "global.h"

bool GlobalData::_instance = false;
GlobalData *GlobalData::_single = nullptr;

GlobalData::GlobalData()
{
    this->_tmp_dir = new QTemporaryDir(QDir::tempPath() + "/TerrainGenerator_XXXXXX");
}

GlobalData::~GlobalData()
{
    _instance = false;
}

GlobalData *GlobalData::getInstance()
{
    if (!_instance)
    {
        _instance = true;
        _single = new GlobalData();
    }
    return _single;
}

QString GlobalData::tmpDir()
{
    return this->_tmp_dir->path();
}