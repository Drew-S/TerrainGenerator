#include "opengl.h"

#include <QOpenGLFunctions>
#include <QOpenGLContext>

OpenGL::OpenGL(QWidget *parent) : QOpenGLWidget(parent) {}

OpenGL::~OpenGL() {}

void OpenGL::initializeGL()
{
    // Get the open gl functions. f->* is the same as the "raw" opengl functions with f-> prefixed on
    // glClearColor(...) in plain openGL and c++ is the same as f->glClearColor in QOpenGL
    // QT's currentContext functions produce a generic set of functions that should be compatible with
    // many different versions of openGL independent of the users graphics card (within reason)
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void OpenGL::resizeGL(int w, int h)
{
    (void)w;
    (void)h;
}

void OpenGL::paintGL()
{
}