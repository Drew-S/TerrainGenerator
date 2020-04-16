#include "opengl.h"

#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QPoint>

#include <GL/gl.h>

// TODO: Added openGL support checks and warning information to user
OpenGL::OpenGL(QWidget *parent) : QOpenGLWidget(parent)
{
    this->_terrain = new Terrain(8);
    this->_camera = new Camera;
    this->_light = new Light;

    // Set default camera location
    this->_camera->setRotationX(45.0f);
    this->_camera->setRotationY(45.0f);
    this->_camera->setZoom(8.0f);

    // Set default light angle
    this->_light->setRotationX(45.0f);
    this->_light->setRotationY(90.0f);
}

OpenGL::~OpenGL()
{
    delete this->_terrain;
    delete this->_camera;
    delete this->_light;
}

// Initialize OpenGL functions and objects
// TODO: add light
void OpenGL::initializeGL()
{
    // Get the open gl functions. f->* is the same as the "raw" opengl functions with f-> prefixed on
    // glClearColor(...) in plain openGL and c++ is the same as f->glClearColor in QOpenGL
    // QT's currentContext functions produce a generic set of functions that should be compatible with
    // many different versions of openGL independent of the users graphics card (within reason)
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Initialize terrain, lights, and camera
    this->_terrain->initializeGL();
}

// On widget being resize, recalculate the camera perspective
void OpenGL::resizeGL(int w, int h)
{
    this->_camera->resize(w, h);
}

// Paint the scene, lights, objects, etc.
// TODO: add light
void OpenGL::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    // Clear screen
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    f->glClear(GL_COLOR_BUFFER_BIT);

    // Draw terrain
    // TODO: Include light parameter
    this->_terrain->paintGL(f, this->_camera->matrix());
}

// On mouse wheel activity, zoom in (was implemented early to debug OpenGL implementation)
void OpenGL::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() < 0)
        this->_camera->zoom(0.25f);
    else
        this->_camera->zoom(-0.25f);
    this->update();
}