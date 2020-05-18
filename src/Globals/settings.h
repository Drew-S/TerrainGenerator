#pragma once

#include <vector>

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

    // Getters (for most if not all settings)
    QDir tmpDir();
    std::vector<QDir> getAssetDirectories();

    bool renderMode();
    int previewResolution();
    int renderResolution();
    int meshResolution();

    // Setters (for settings that can be changed)
    void setRenderMode(bool mode);
    void setPreviewResolution(int resolution);
    void setRenderResolution(int resolution);
    void setMeshResolution(int resolution);

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
    int _preview_resolution = 512; // Image resolution during design
    int _render_resolution = 1024; // Image resolution when rendering/exporting
    bool _render_mode = false;     // Whether to use render resolution or preview
};

#define SETTINGS Settings::getInstance()
