#pragma once

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

// Custom OPenGL widget that manages and renders the OpenGL context to the window
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
};