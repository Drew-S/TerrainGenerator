#pragma once

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QSlider>
#include <QDial>

#include "./Terrain/terrain.h"
#include "./Camera/camera.h"
#include "./Light/light.h"

// Custom OpenGL widget that manages and renders the OpenGL context to the widget
class OpenGL : public QOpenGLWidget
{
public:
    explicit OpenGL(QWidget *parent = 0);
    ~OpenGL();

public slots:
    void sunRotationX(int value);
    void sunRotationY(int value);

    void camRotationX(int value);
    void camRotationY(int value);
    void camZoom(int value);

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
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    // Terrain handler
    Terrain *_terrain;
    // Camera handler
    Camera *_camera;
    // Light handler
    Light *_light;

    // Switch to update data when mouse move is click drag
    bool _dragging = false;

    // Previous position for createing relative mouse movement data in mouseMoveEvent
    QPoint _prev;

    QWidget *_controls;

    QSlider *_control_cam_zoom;
    QSlider *_control_cam_rotation_x;
    QDial *_control_cam_rotation_y;

    QSlider *_control_sun_rotation_x;
    QDial *_control_sun_rotation_y;
};