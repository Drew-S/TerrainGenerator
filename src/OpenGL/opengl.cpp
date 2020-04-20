#include "opengl.h"

#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QPoint>

#include <GL/gl.h>

// TODO: add ui support for selecting sky color
#define SKY 0.77f, 0.84f, 0.99f, 1.0f

// TODO: Add openGL support checks and warning information to user
// Creates an OpenGL widget (QOpenGLWidget)
OpenGL::OpenGL(QWidget *parent) : QOpenGLWidget(parent)
{
    // Set surface format detail
    QSurfaceFormat fmt;

    // Set antialias samples
    fmt.setSamples(10);

    // Set depth buffer size
    fmt.setDepthBufferSize(48);

    // Set format for OpenGL widget to use
    this->setFormat(fmt);

    // Create a terrain object with a set resolution
    // TODO: add support for changing terrain resolution
    // WARNING: Maxed out at around 256^2 vertices
    this->_terrain = new Terrain(256);

    // Create camera
    this->_camera = new Camera;

    // Create light
    this->_light = new Light;

    // Set default camera rotation and zoom
    this->_camera->setRotationX(45.0f);
    this->_camera->setZoom(4.243f);

    // Set default light rotation
    this->_light->setRotationX(45.0f);
}

// Clean up objects
OpenGL::~OpenGL()
{
    delete this->_terrain;
    delete this->_camera;
    delete this->_light;
}

// Initialize OpenGL functions and objects
void OpenGL::initializeGL()
{
    // Get the open gl functions. f->* is the same as the "raw" opengl functions with f-> prefixed on
    // glClearColor(...) in plain openGL and c++ is the same as f->glClearColor in QOpenGL
    // QT's currentContext functions produce a generic set of functions that should be compatible with
    // many different versions of openGL independent of the users graphics card (within reason)
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->initializeOpenGLFunctions();

    // Enable depth test
    f->glEnable(GL_DEPTH_TEST);
    f->glDepthFunc(GL_LESS);

    // Set sky color
    f->glClearColor(SKY);

    // Initialize terrain's OpenGL
    this->_terrain->initializeGL();
}

// On widget being resize, recalculate the camera perspective
void OpenGL::resizeGL(int w, int h)
{
    this->_camera->resize(w, h);
}

// Paint the scene, lights, objects, etc.
void OpenGL::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    // Clear screen
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set sky color
    f->glClearColor(SKY);

    // Get camera details
    QMatrix4x4 camera_matrix = this->_camera->matrix();
    QVector3D camera_pos = this->_camera->position();

    // Get light details
    QVector3D light_color = this->_light->color();
    QVector3D light_pos = this->_light->position();
    float light_intensity = this->_light->intensity();

    // Draw terrain
    this->_terrain->paintGL(
        f,
        camera_matrix,
        camera_pos,
        light_color,
        light_pos,
        light_intensity);

    // Draw the sun graphic
    this->_light->paintGL(camera_matrix);

    // Draw additional debug info (axis)
    // TODO: Draw axis as an info graphic in the corner

    float length = 2.0f;

    // Get the axis vectors for drawing the world space zero axis compensated for the camera
    QVector3D x = camera_matrix * QVector3D(length, 0.0f, 0.0f);   // (1,  0,  0)
    QVector3D y = camera_matrix * QVector3D(0.0f, length, 0.0f);   // (0,  1,  0)
    QVector3D z = camera_matrix * QVector3D(0.0f, 0.0f, length);   // (0,  0,  1)
    QVector3D xn = camera_matrix * QVector3D(-length, 0.0f, 0.0f); // (-1, 0,  0)
    QVector3D yn = camera_matrix * QVector3D(0.0f, -length, 0.0f); // (0, -1,  0)
    QVector3D zn = camera_matrix * QVector3D(0.0f, 0.0f, -length); // (0,  0, -1)
    QVector3D zero = camera_matrix * QVector3D(0.0f, 0.0f, 0.0f);  // (0,  0,  0)

    // Draw the origin point (black dot at world (0, 0, 0))
    glBegin(GL_POINTS);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(zero.x(), zero.y(), zero.z());
    glEnd();

    // Draw positive x-axis
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(x.x(), x.y(), x.z());
    glVertex3f(zero.x(), zero.y(), zero.z());

    // Draw positive y-axis
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(y.x(), y.y(), y.z());
    glVertex3f(zero.x(), zero.y(), zero.z());

    // Draw positive z-axis
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(z.x(), z.y(), z.z());
    glVertex3f(zero.x(), zero.y(), zero.z());
    glEnd();

    // Draw dashed lines
    glPushAttrib(GL_ENABLE_BIT);

    glLineStipple(4, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);

    // Draw negative x-axis
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(xn.x(), xn.y(), xn.z());
    glVertex3f(zero.x(), zero.y(), zero.z());

    // Draw negative y-axis
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(yn.x(), yn.y(), yn.z());
    glVertex3f(zero.x(), zero.y(), zero.z());

    // Draw negative z-axis
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(zn.x(), zn.y(), zn.z());
    glVertex3f(zero.x(), zero.y(), zero.z());
    glEnd();

    glPopAttrib();
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

void OpenGL::mousePressEvent(QMouseEvent *event)
{
    this->_prev = event->pos();
    this->_dragging = true;
}

void OpenGL::mouseReleaseEvent(QMouseEvent *event)
{
    (void *)event;
    this->_dragging = false;
}

void OpenGL::mouseMoveEvent(QMouseEvent *event)
{
    Qt::KeyboardModifiers modifiers = event->modifiers();

    QPoint delta = this->_prev - event->pos();
    this->_prev = event->pos();

    if (modifiers.testFlag(Qt::ShiftModifier))
    {
        this->_light->rotateX(delta.y());
        this->_light->rotateY(delta.x());
    }
    else
    {
        this->_camera->rotateX(-delta.y());
        this->_camera->rotateY(-delta.x());
    }
    update();
}