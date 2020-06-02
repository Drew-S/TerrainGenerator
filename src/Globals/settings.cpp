#include "settings.h"

#include <QDebug>

#define Q_BETWEEN(low, v, hi) Q_ASSERT(low <= v && v <= hi)

// Setup data for singleton
bool Settings::_instance = false;
Settings *Settings::_single = nullptr;

// Create settings and setup default data
Settings::Settings()
{
    this->_tmp_dir = new QTemporaryDir(QDir::cleanPath(QDir::tempPath() + QString("/TerrainGenerator_XXXXXX")));

#if (defined(DEVELOPMENT_MODE) || defined(TEST_MODE))
    this->_system_asset_directory = QDir(QDir::cleanPath(QString(PWD) + QString("/assets")));
    qDebug("Using development asset directory '%s'", qPrintable(this->_system_asset_directory.path()));
#else
#ifdef __linux
    this->_system_asset_directory = QDir("/usr/share/TerrainGenerator");
    if (qgetenv("XDG_CONFIG_HOME") != "")
        this->_user_asset_directory = QDir(QDir::cleanPath(qgetenv("XDG_CONFIG_HOME") + QString("/TerrainGenerator")));
    else
        this->_user_asset_directory = QDir(QDir::cleanPath(QDir::homePath() + QString("/.TerrainGenerator")));
#elif __WIN32
    // Need to verify
    this->_system_asset_directory = QDir("C:/Program Files/TerrainGenerator");
    this->_user_asset_directory = QDir(QDir::cleanPath(qgetenv("APPDATA")));
#elif __APPLE__
    // Not defined, never used apple so need to look up differences
#endif
    qDebug("Using system asset directory '%s'", qPrintable(this->_system_asset_directory.path()));
    qDebug("Using user asset directory '%s'", qPrintable(this->_user_asset_directory.path()));
#endif
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
    Q_CHECK_PTR(_single);
    return _single;
}

// Get the temp directory
QDir Settings::tmpDir()
{
    Q_CHECK_PTR(this->_tmp_dir);
    return QDir(this->_tmp_dir->path());
}

std::vector<QDir> Settings::getAssetDirectories()
{
    std::vector<QDir> assets;
    assets.push_back(this->_system_asset_directory);

#if (!defined(DEVELOPMENT_MODE) && !defined(TEST_MODE))
    assets.push_back(this->_user_asset_directory);
#endif

    return assets;
}

bool Settings::renderMode()
{
    return this->_render_mode;
}

int Settings::previewResolution()
{
    Q_BETWEEN(1, this->_preview_resolution, 8192);
    return this->_preview_resolution;
}

int Settings::renderResolution()
{
    Q_BETWEEN(1, this->_render_resolution, 8192);
    return this->_render_resolution;
}

int Settings::meshResolution()
{
    Q_BETWEEN(1, this->_mesh_resolution, 256);
    return this->_mesh_resolution;
}

bool Settings::packImages()
{
    return this->_pack_images;
}

bool Settings::percentProgressText()
{
    return this->_percent_progress_text;
}

void Settings::setRenderMode(bool mode)
{
    this->_render_mode = mode;
    qDebug("Render mode changed to %s", mode ? "true" : "false");

// Signals from settings are causing fatal errors during testing,
// signals in other files work fine, signals in settings work fine during normal
// operation
#ifndef TEST_MODE
    emit this->renderModeChanged(mode);
#endif
}

void Settings::setPreviewResolution(int resolution)
{
    this->_preview_resolution = resolution < 1 ? 1 : (resolution > 8192 ? 8192 : resolution);
    Q_BETWEEN(1, this->_preview_resolution, 8192);
    qDebug("Preview Resolution changed %d", this->_preview_resolution);
#ifndef TEST_MODE
    emit this->previewResolutionChanged(this->_preview_resolution);
#endif
}

void Settings::setRenderResolution(int resolution)
{
    this->_render_resolution = resolution < 1 ? 1 : (resolution > 8192 ? 8192 : resolution);
    Q_BETWEEN(1, this->_render_resolution, 8192);
    qDebug("Preview Resolution changed %d", this->_render_resolution);
#ifndef TEST_MODE
    emit this->renderResolutionChanged(this->_render_resolution);
#endif
}

void Settings::setMeshResolution(int resolution)
{
    this->_mesh_resolution = resolution < 1 ? 1 : (resolution > 256 ? 265 : resolution);
    Q_BETWEEN(1, this->_mesh_resolution, 256);
    qDebug("Preview Resolution changed %d", this->_mesh_resolution);
#ifndef TEST_MODE
    emit this->meshResolutionChanged(this->_mesh_resolution);
#endif
}

void Settings::setPackImages(bool mode)
{
    this->_pack_images = mode;
    qDebug("Pack images (%s)?", this->_pack_images ? "true" : "false");
#ifndef TEST_MODE
    emit this->packImagesChanged(this->_pack_images);
#endif
}

void Settings::setPercentProgressText(bool mode)
{
    this->_percent_progress_text = mode;
    qDebug("Show progress bar percents (%s)?", this->_percent_progress_text ? "true" : "false");
#ifndef TEST_MODE
    emit this->percentProgressTextChanged(this->_percent_progress_text);
#endif
}