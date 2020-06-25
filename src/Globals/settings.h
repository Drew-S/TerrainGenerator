#pragma once

#include <vector>

#include <QColor>
#include <QDir>
#include <QObject>
#include <QTemporaryDir>

// Global singleton for sharing settings throughout
/**
 * Settings
 * 
 * Singleton for housing global settings (project and application) that can be
 * accessed from anywhere. Houses getters, setters, and signals for global
 * settings.
 */
class Settings : public QObject
{
    Q_OBJECT
public:
    // Get/create a reference to the settings instance
    static Settings *getInstance();
    ~Settings();

    // Getters
    QDir tmpDir(); // Get only
    std::vector<QDir> getAssetDirectories(); // Get only
    QDir getDocsDirectory(); // Get only

    bool renderMode();
    int previewResolution();
    int renderResolution();
    int meshResolution();
    // QColor skyColor();
    // QColor sunColor();
    // QColor terrainColor();
    // QColor terranLinesColor();
    bool packImages();
    bool percentProgressText();
    bool runRender();

    // Setters
    void setRenderMode(bool mode);
    void setPreviewResolution(int resolution);
    void setRenderResolution(int resolution);
    void setMeshResolution(int resolution);
    // void skyColor(QColor color);
    // void sunColor(QColor color);
    // void terrainColor(QColor color);
    // void terranLinesColor(QColor color);
    void setPackImages(bool mode);
    void setPercentProgressText(bool mode);
    void setRunRender(bool mode);

    // QJsonObject save() const;
    // void restore(QJsonObject const &data);

signals:
    // Signals for when a value is changed through setters
    void renderModeChanged(bool mode);
    void previewResolutionChanged(int resolution);
    void renderResolutionChanged(int resolution);
    void meshResolutionChanged(int resolution);
    void packImagesChanged(bool mode);
    void percentProgressTextChanged(bool mode);
    void runRenderChanged(bool mode);

private:
    Settings();
    static bool _instance;
    static Settings *_single;

    // Settings
    QTemporaryDir *_tmp_dir;        // /tmp/TerrainGenerator_XXXXXX/
    QDir _system_asset_directory;   // /usr/share/TerrainGenerator/assets...
    QDir _user_asset_directory;     // /home/<user>/.TerrainGenerator/asset...
    QDir _doc_directory;            // /usr/share/TerrainGenerator/docs...
    int _mesh_resolution = 256;     // Vertices on OpenGL preview mesh
    int _preview_resolution = 256;  // Image resolution during design
    int _render_resolution = 1024;  // Image resolution when rendering/exporting
    bool _render_mode = false;      // Whether to use render resolution or not
    bool _run_render = false; // Whether running export render to save files
    // QColor _sun{255, 255, 255};     // Sun light colour in OpenGL window
    // QColor _sky{196, 214, 252};     // Sky/ambient light colour in OpenGL window
    // QColor _terrain{191, 191, 191}; // Terrain colour in OpenGL window
    // QColor _lines{255, 255, 255};   // Terrain line colour in OpenGL window
    bool _pack_images = false;
    bool _percent_progress_text = false;
};

#define SETTINGS Settings::getInstance()
