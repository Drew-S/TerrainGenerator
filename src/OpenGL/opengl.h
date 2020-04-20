#pragma once

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPoint>

#include "./Terrain/terrain.h"
#include "./Camera/camera.h"
#include "./Light/light.h"

// Custom OpenGL widget that manages and renders the OpenGL context to the widget
class OpenGL : public QOpenGLWidget
{
public:
    explicit OpenGL(QWidget *parent = 0);
    ~OpenGL();

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
};