#pragma once

#include <QDial>
#include <QImage>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QPoint>
#include <QSlider>
#include <QWheelEvent>
#include <QWidget>

#include "./camera.h"
#include "./light.h"
#include "./terrain.h"

/**
 * OpenGL
 * 
 * Custom OpenGL management widget over the QOpenGLWidget. Manages the terrain
 * light and cameras. Displays the resulting widget
 */
class OpenGL : public QOpenGLWidget
{
public:
    // Create the widget
    explicit OpenGL(QWidget *parent = 0);

    // Delete the widget
    ~OpenGL();

    // Set terrain values
    // TODO: Use SETTINGS instead
    void setTerrainDrawLines(bool lines);
    void setTerrainColor(QColor color);
    void setTerrainLineColor(QColor color);
    void setTerrainMeshResolution(int resolution);

    // Set light values
    void setSkyColor(QColor color);
    void setLightColor(QColor color);

    // Get values
    bool terrainDrawLines();
    QColor terrainColor();
    QColor terrainLineColor();
    QColor skyColor();
    QColor lightColor();

public slots:
    // Called when the sun widgets are adjusted
    void sunRotationX(int value);
    void sunRotationY(int value);

    // Called when the camera widgets are adjusted
    void camRotationX(int value);
    void camRotationY(int value);
    void camZoom(int value);

    // Called when the nodeeditor has updated normal and height maps
    void nodeeditorOutputUpdated(QImage normal_map,
                                 QImage height_map,
                                 QImage albedo_map);

protected:
    // Initialize gl functions and settings
    void initializeGL() override;
    // Update the widget viewport and camera
    void resizeGL(int w, int h) override;
    // Paint the scene
    void paintGL() override;

    // On mouse wheel scroll, zoom in/out
    void wheelEvent(QWheelEvent *event) override;

    // Mouse click and draw rotate camera, shift to rotate light
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QVector3D _ambient{0.77, 0.84, 0.99};
    // Terrain handler
    Terrain *_terrain;
    // Camera handler
    Camera *_camera;
    // Light handler
    Light *_light;

    // Previous position for createing relative mouse movement data in
    // mouseMoveEvent
    QPoint _prev;

    // Pointer for the overlay controls
    QWidget *_controls;

    // Pointers for specific overlay controls, used for listeners and updating
    QSlider *_control_cam_zoom;
    QSlider *_control_cam_rotation_x;
    QDial *_control_cam_rotation_y;

    QSlider *_control_sun_rotation_x;
    QDial *_control_sun_rotation_y;
};