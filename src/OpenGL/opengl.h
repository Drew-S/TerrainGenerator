#pragma once

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QWheelEvent>

#include "./Terrain/terrain.h"
#include "./Camera/camera.h"

// Custom OPenGL widget that manages and renders the OpenGL context to the window
class OpenGL : public QOpenGLWidget
{
public:
    explicit OpenGL(QWidget *parent = 0);
    ~OpenGL(){};

    // TODO: Add event handlers for pan/zoom camera and pan/zoom light
protected:
    // Initialize gl functions and settings
    void initializeGL() override;
    // Update the widget viewport and camera
    void resizeGL(int w, int h) override;
    // Paint the scene
    void paintGL() override;

    // On mouse wheel scroll, zoom in/out
    void wheelEvent(QWheelEvent *event) override;

private:
    Terrain *_terrain;
    Camera *_camera;
    // Light *_light;
};