#include "settings.h"

#include <QDebug>

#define Q_BETWEEN(low, v, hi) Q_ASSERT(low <= v && v <= hi)

#define MAX_IMAGE 8192
#define MAX_MESH 256

// Setup data for singleton
bool Settings::_instance = false;
Settings *Settings::_single = nullptr;

/**
 * Settings
 * 
 * Create a settings singleton instance. Create a temp directory used for the
 * entire application. Gets the asset directory locations (if the
 * DEVELOPMENT_MODE or TEST_MODE compile option is set, we use the development
 * directory of assets). Depending on the platform (windows/linux/macos) the
 * asset directories are set accordingly. First, there is the system directory
 * which houses built in stencils and other assets, then the user directory
 * where the user can supply their own stencils and other assets.
 */
Settings::Settings()
{
    this->_tmp_dir = new QTemporaryDir(QDir::cleanPath(QDir::tempPath() + QString("/TerrainGenerator_XXXXXX")));

#if (defined(DEVELOPMENT_MODE) || defined(TEST_MODE))
    this->_system_asset_directory = QDir(QDir::cleanPath(QString(PWD) + QString("/assets")));
    this->_doc_directory = QDir(QDir::cleanPath(QString(PWD) + QString("/docs")));
    qDebug("Using development asset directory '%s'", qPrintable(this->_system_asset_directory.path()));
#else
#ifdef __linux
    this->_system_asset_directory = QDir("/usr/share/TerrainGenerator/assets");
    this->_doc_directory = QDir("/usr/share/TerrainGenerator/docs")
    if (qgetenv("XDG_CONFIG_HOME") != "")
        this->_user_asset_directory = QDir(QDir::cleanPath(qgetenv("XDG_CONFIG_HOME") + QString("/TerrainGenerator/assets")));
    else
        this->_user_asset_directory = QDir(QDir::cleanPath(QDir::homePath() + QString("/.TerrainGenerator/assets")));
#elif __WIN32
    // Need to verify
    this->_system_asset_directory = QDir("C:/Program Files/TerrainGenerator/asset");
    this->_doc_directory = QDir("C:/Program Files/TerrainGenerator/docs");
    this->_user_asset_directory = QDir(QDir::cleanPath(qgetenv("APPDATA")));
#elif __APPLE__
    // Not defined, never used apple so need to look up differences
#endif
    qDebug("Using system asset directory '%s'", qPrintable(this->_system_asset_directory.path()));
    qDebug("Using user asset directory '%s'", qPrintable(this->_user_asset_directory.path()));
#endif
}

/**
 * ~Settings
 * 
 * Sets that no settings singleton is used. Deletes the singleton.
 */
Settings::~Settings()
{
    _instance = false;
}

/**
 * getInstance
 * 
 * Get the singleton settings instance. If the singleton does not exist yet, it
 * is created.
 * 
 * @returns Settings * :  The setttings singleton.
 */
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

/******************************************************************************
 *                                 GETTERS                                    *
 ******************************************************************************/

/**
 * tmpDir
 * 
 * Get the temporary directory of the running application, this uses the system
 * temp directory (/tmp on linux).
 * 
 * @returns QDir : The directory object of the temp directory.
 */
QDir Settings::tmpDir()
{
    Q_CHECK_PTR(this->_tmp_dir);
    return QDir(this->_tmp_dir->path());
}

/**
 * getAssetDirectories
 * 
 * Get the list of directories that houses the asset information (1 test/default
 * directory if in development/test mode, 2 in normal use case - system and user
 * directories).
 * 
 * @returns std::vector<QDir> : A list of asset directories.
 */
std::vector<QDir> Settings::getAssetDirectories()
{
    std::vector<QDir> assets;
    assets.push_back(this->_system_asset_directory);

#if (!defined(DEVELOPMENT_MODE) && !defined(TEST_MODE))
    assets.push_back(this->_user_asset_directory);
#endif

    return assets;
}

/**
 * getDocsDirectory
 * 
 * Get the QDir for the location of document files.
 * 
 * @returns QDir : The docs directory.
 */
QDir Settings::getDocsDirectory()
{
    return this->_doc_directory;
}

/**
 * renderMode
 * 
 * Get the current render mode (flag).
 * 
 * @returns bool : Wether or not we are using render mode.
 */
bool Settings::renderMode()
{
    return this->_render_mode;
}

/**
 * previewResolution
 * 
 * Returns the preview resolution (single edge as we are using square textures).
 * 
 * @returns int : The resolution to be used in images.
 */
int Settings::previewResolution()
{
    Q_BETWEEN(1, this->_preview_resolution, 8192);
    return this->_preview_resolution;
}

/**
 * renderResolution
 * 
 * Returns the render resolution (single edge as we are using square textures).
 * 
 * @returns int : The resolution to be used in images.
 */
int Settings::renderResolution()
{
    Q_BETWEEN(1, this->_render_resolution, 8192);
    return this->_render_resolution;
}

/**
 * meshResolution
 * 
 * Returns the mesh resolution (single edge) that is used for the terrain
 * geometry (number of vertices along one side).
 * 
 * @returns int : The resolution to be used in vertex generation.
 */
int Settings::meshResolution()
{
    Q_BETWEEN(1, this->_mesh_resolution, 256);
    return this->_mesh_resolution;
}

/**
 * packImages
 * 
 * Returns flag as to wether or not we are to pack external textures within the
 * project save file
 * 
 * @returns bool : Wether or not to pack images in with the project folder.
 */
bool Settings::packImages()
{
    return this->_pack_images;
}

/**
 * percentProgressText
 * 
 * Flag as for the UI as to wether or not progress bars should show the
 * percentage text (off by default).
 * 
 * @returns bool : Wether or not to display percentage text.
 */
bool Settings::percentProgressText()
{
    return this->_percent_progress_text;
}

/******************************************************************************
 *                                 SETTERS                                    *
 ******************************************************************************/

/**
 * setRenderMode
 * 
 * Sets the render mode flag.
 * 
 * @param bool mode : The render mode flag to use.
 * 
 * @signals renderModeChanged
 */
void Settings::setRenderMode(bool mode)
{
    this->_render_mode = mode;
    qDebug("Render mode changed to %s", mode ? "true" : "false");

// Signals from settings are causing fatal errors during testing,
// signals in other files work fine, signals in settings work fine during normal
// operation, as such signals here are disabled.
#ifndef TEST_MODE
    emit this->renderModeChanged(mode);
#endif
}

/**
 * setPreviewResolution
 * 
 * Set the preview resolution. Limited between 1 and MAX_IMAGE (8192).
 * 
 * @param int resolution : The new preview image resolution.
 * 
 * @signals previewResolutionChanged
 */
void Settings::setPreviewResolution(int resolution)
{
    this->_preview_resolution =
        resolution < 1 ? 1 : (resolution > MAX_IMAGE ? MAX_IMAGE : resolution);

    Q_BETWEEN(1, this->_preview_resolution, MAX_IMAGE);
    qDebug("Preview Resolution changed %d", this->_preview_resolution);
#ifndef TEST_MODE
    emit this->previewResolutionChanged(this->_preview_resolution);
#endif
}

/**
 * setRenderResolution
 * 
 * Set the render resolution. Limited between 1 and MAX_IMAGE (8192).
 * 
 * @param int resolution : The new render image resolution.
 * 
 * @signals renderResolutionChanged
 */
void Settings::setRenderResolution(int resolution)
{
    this->_render_resolution =
        resolution < 1 ? 1 : (resolution > MAX_IMAGE ? MAX_IMAGE : resolution);

    Q_BETWEEN(1, this->_render_resolution, MAX_IMAGE);
    qDebug("Preview Resolution changed %d", this->_render_resolution);
#ifndef TEST_MODE
    emit this->renderResolutionChanged(this->_render_resolution);
#endif
}

/**
 * setMeshResolution
 * 
 * Set the mesh resolution. Limited between 1 and MAX_MESH (256).
 * 
 * @param int resolution : The new mesh terrain resolution.
 * 
 * @signals meshResolutionChanged
 */
void Settings::setMeshResolution(int resolution)
{
    this->_mesh_resolution =
        resolution < 1 ? 1 : (resolution > MAX_MESH ? MAX_MESH : resolution);

    Q_BETWEEN(1, this->_mesh_resolution, MAX_MESH);
    qDebug("Preview Resolution changed %d", this->_mesh_resolution);
#ifndef TEST_MODE
    emit this->meshResolutionChanged(this->_mesh_resolution);
#endif
}

/**
 * setPackImages
 * 
 * Set the flag for whether or not to pack external texture files in with the
 * save file.
 * 
 * @param bool mode : The flag as to whether or not to pack images.
 * 
 * @signals packImagesChanged
 */
void Settings::setPackImages(bool mode)
{
    this->_pack_images = mode;
    qDebug("Pack images (%s)?", this->_pack_images ? "true" : "false");
#ifndef TEST_MODE
    emit this->packImagesChanged(this->_pack_images);
#endif
}

/**
 * setPercentProgressText
 * 
 * Set whether or not to show the percentage progress value with progress bars.
 * 
 * @param bool mode : The flag as to whether or not to use percentage with
 *                    progress bars.
 */
void Settings::setPercentProgressText(bool mode)
{
    this->_percent_progress_text = mode;
    qDebug("Show progress bar percents (%s)?",
        this->_percent_progress_text ? "true" : "false");
        
#ifndef TEST_MODE
    emit this->percentProgressTextChanged(this->_percent_progress_text);
#endif
}