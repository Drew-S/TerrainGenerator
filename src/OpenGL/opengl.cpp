#include "opengl.h"

#include <QOpenGLFunctions>
#include <QOpenGLContext>

#include <GL/gl.h>

OpenGL::OpenGL(QWidget *parent) : QOpenGLWidget(parent)
{
    // TODO: create camera and light objects
    this->_terrain = new Terrain(1);
}

OpenGL::~OpenGL() {}

// Initialize OpenGL functions and objects
// TODO: add camera, add light
void OpenGL::initializeGL()
{
    // Get the open gl functions. f->* is the same as the "raw" opengl functions with f-> prefixed on
    // glClearColor(...) in plain openGL and c++ is the same as f->glClearColor in QOpenGL
    // QT's currentContext functions produce a generic set of functions that should be compatible with
    // many different versions of openGL independent of the users graphics card (within reason)
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->initializeOpenGLFunctions();
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Initialize terrain, lights, and camera
    this->_terrain->initializeGL();
}

// On widget being resize, recalculate the camera
// TODO: Implement camera
void OpenGL::resizeGL(int w, int h)
{
    (void)w;
    (void)h;
}

// Paint the scene, lights, objects, etc.
// TODO: add camera, add light
void OpenGL::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    f->glClear(GL_COLOR_BUFFER_BIT);
    this->_terrain->paintGL(f);
}