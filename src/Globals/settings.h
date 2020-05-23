#pragma once

#include <vector>

#include <QDir>
#include <QTemporaryDir>
#include <QObject>
#include <QColor>

// Global singleton for sharing settings throughout
class Settings : public QObject
{
    Q_OBJECT
public:
    // Get/create a reference to the settings instance
    static Settings *getInstance();
    ~Settings();

    // Getters (for most if not all settings)
    QDir tmpDir();
    std::vector<QDir> getAssetDirectories();

    bool renderMode();
    int previewResolution();
    int renderResolution();
    int meshResolution();
    // QColor skyColor();
    // QColor sunColor();
    // QColor terrainColor();
    // QColor terranLinesColor();

    // Setters (for settings that can be changed)
    void setRenderMode(bool mode);
    void setPreviewResolution(int resolution);
    void setRenderResolution(int resolution);
    void setMeshResolution(int resolution);
    // void skyColor(QColor color);
    // void sunColor(QColor color);
    // void terrainColor(QColor color);
    // void terranLinesColor(QColor color);

    // QJsonObject save() const;
    // void restore(QJsonObject const &data);

signals:
    void renderModeChanged(bool mode);
    void previewResolutionChanged(int resolution);
    void renderResolutionChanged(int resolution);
    void meshResolutionChanged(int resolution);

private:
    Settings();
    static bool _instance;
    static Settings *_single;

    // Settings
    QTemporaryDir *_tmp_dir;       // /tmp/TerrainGenerator_XXXXXX/
    QDir _system_asset_directory;  // /user/share/TerrainGenerator/...
    QDir _user_asset_directory;    // /home/<user>/.TerrainGenerator/...
    int _mesh_resolution = 256;    // Vertices on OpenGL preview mesh
    int _preview_resolution = 256; // Image resolution during design
    int _render_resolution = 1024; // Image resolution when rendering/exporting
    bool _render_mode = false;     // Whether to use render resolution or preview
    // QColor _sun{255, 255, 255};     // Sun light colour in OpenGL window
    // QColor _sky{196, 214, 252};     // Sky/ambient light colour in OpenGL window
    // QColor _terrain{191, 191, 191}; // Terrain colour in OpenGL window
    // QColor _lines{255, 255, 255};   // Terrain line colour in OpenGL window
};

#define SETTINGS Settings::getInstance()
