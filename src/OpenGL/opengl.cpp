#include "opengl.h"

#include "ui_OpenGLControls.h"

#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QPoint>
#include <QPointF>
#include <QMatrix>
#include <QDebug>

#include <GL/gl.h>

// TODO: add ui support for selecting sky color
#define SKY 0.77f, 0.84f, 0.99f, 1.0f

// TODO: Add openGL support checks and warning information to user
// Creates an OpenGL widget (QOpenGLWidget)
OpenGL::OpenGL(QWidget *parent) : QOpenGLWidget(parent)
{
    qDebug("Setting up OpenGL widget");
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
    float cam_rotation_x = this->_camera->setRotationX(45.0f);
    float cam_rotation_y = this->_camera->setRotationY(0.0f);
    float cam_zoom = this->_camera->setZoom(4.25f);

    // Set default light rotation
    float sun_rotation_x = this->_light->setRotationX(45.0f);
    float sun_rotation_y = this->_light->setRotationY(0.0f);

    // Attach overlay controls
    this->_controls = new QWidget(this);

    Ui::OpenGLControls ui;
    ui.setupUi(this->_controls);

    // Get camera's current settings
    this->_control_cam_zoom = ui.cam_zoom;
    this->_control_cam_rotation_x = ui.cam_rotation_x;
    this->_control_cam_rotation_y = ui.cam_rotation_y;

    // Get lights current settings
    this->_control_sun_rotation_x = ui.sun_rotation_x;
    this->_control_sun_rotation_y = ui.sun_rotation_y;

    // Set camera control values
    this->_control_cam_zoom->setSliderPosition((int)(4.0f * cam_zoom));
    this->_control_cam_rotation_x->setSliderPosition((int)cam_rotation_x);
    this->_control_cam_rotation_y->setSliderPosition((int)cam_rotation_y);

    // Set light control values
    this->_control_sun_rotation_x->setSliderPosition(sun_rotation_x);
    this->_control_sun_rotation_y->setSliderPosition(sun_rotation_y);

    // Attach listeners for the camera controls
    QObject::connect(this->_control_cam_rotation_x, &QSlider::valueChanged, this, &OpenGL::camRotationX);
    QObject::connect(this->_control_cam_rotation_y, &QSlider::valueChanged, this, &OpenGL::camRotationY);
    QObject::connect(this->_control_cam_zoom, &QSlider::valueChanged, this, &OpenGL::camZoom);

    // Attach listeners for the sun controls
    QObject::connect(this->_control_sun_rotation_x, &QSlider::valueChanged, this, &OpenGL::sunRotationX);
    QObject::connect(this->_control_sun_rotation_y, &QSlider::valueChanged, this, &OpenGL::sunRotationY);
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
    qDebug("Initializing OpenGL functionality");
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

// On widget being resize, recalculate the camera perspective, and update the control widget size
void OpenGL::resizeGL(int w, int h)
{
    this->_camera->resize(w, h);
    this->_controls->setGeometry(0, -10, w, h);
}

// Paint the scene, lights, objects, etc.
// TODO: Commenting, improve labeling, describe whats happening
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

    // Current widget width and height
    float width = (float)this->size().width();
    float height = (float)this->size().height();

    // How large (pixels) should the axis indicator be (WxH)
    float i_size = 120.0f;
    // How far from the corner of the screen (pixels)
    float margin = 25.0f;

    // Get the camera rotation matrix
    QMatrix4x4 rotation_matrix = this->_camera->rotationMatrix();

    // Get the axis vectors for drawing the world space zero axis compensated for the camera rotation
    // 3D vector form
    QVector3D x_v = rotation_matrix * QVector3D(1.0f, 0.0f, 0.0f);    // (1,  0,  0)
    QVector3D y_v = rotation_matrix * QVector3D(0.0f, 1.0f, 0.0f);    // (0,  1,  0)
    QVector3D z_v = rotation_matrix * QVector3D(0.0f, 0.0f, 1.0f);    // (0,  0,  1)
    QVector3D zero_v = rotation_matrix * QVector3D(0.0f, 0.0f, 0.0f); // (0,  0,  0)

    // Transform to move and scale the indicator into the corner
    // w = width, h = height, s = i_size, m = margin
    // 1 0 -1 | 1/w 0   0 | 1 0 m | s/2 0   0 | 1 0 1 | x
    // 0 1 -1 | 0   1/h 0 | 0 1 m | 0   s/2 0 | 0 1 1 | y
    // 0 0  1 | 0   0   1 | 0 0 1 | 0   0   1 | 0 0 1 | 1
    QMatrix transform; // (3x3 matrix)
    transform = transform * QMatrix(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
    transform = transform * QMatrix(i_size / 2.0f, 0.0f, 0.0f, i_size / 2.0f, 0.0f, 0.0f);
    transform = transform * QMatrix(1.0f, 0.0f, 0.0f, 1.0f, margin, margin);
    transform = transform * QMatrix(1.0f / width, 0.0f, 0.0f, 1.0f / height, 0.0f, 0.0f);
    transform = transform * QMatrix(1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f);

    // Apply tranformation to the indicator coordinates
    // QPointF is used as QMatrix does not support multiplication by QVector2D
    QPointF x = transform * QPointF(x_v.x(), x_v.y());
    QPointF y = transform * QPointF(y_v.x(), y_v.y());
    QPointF z = transform * QPointF(z_v.x(), z_v.y());
    QPointF zero = transform * QPointF(zero_v.x(), zero_v.y());

    // Compensations for drawing axis over each other, z value was stripped from above,
    // this re-introduces them into the drawing for pseudo-depth.
    // TODO: improve names here
    float z_0 = 0.0f; // z-axis compensation (for "winning" over x-axis)
    float z_1 = 0.2f; // z-axis compensation (for "winning" over x-axis)
    float o = 0.0f;   // Origin compensation (for "winning" over x- and z-axis)

    // z-axis is point to the right of screen, it should be in front of the x-axis
    // x-axis normally dominates
    if ((z - zero).x() >= 0)
    {
        z_0 = 0.01;
        z_1 = 0.1f;
    }

    // If z-axis is pointing right, or x-axis is pointing left (with error margin for point size)
    // Have origin draw over (origin in is front, black dot)
    if ((z - zero).x() > 0.0207912 || (x - zero).x() < -0.0207912)
    {
        o = 0.125f;
    }

    glPointSize(5.0f);
    glLineWidth(3.0f);

    // Draw the origin point (black dot representing (0, 0, 0))
    glBegin(GL_POINTS);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(zero.x(), zero.y(), 0.125f - o);
    glEnd();

    glPointSize(7.0f);

    // Axis end points
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(x.x(), x.y(), 0.1f);

    glColor3f(0.0f, 0.0f, 1.0f - z_0);
    glVertex3f(z.x(), z.y(), 0.1f - z_0);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(y.x(), y.y(), 0.0f);
    glEnd();

    // Draw positive x-axis
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(x.x(), x.y(), 0.2f);
    glVertex3f(zero.x(), zero.y(), 0.2f);

    // Draw positive z-axis
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(z.x(), z.y(), z_1 - z_0);
    glVertex3f(zero.x(), zero.y(), z_1 - z_0);

    // Draw positive y-axis
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(y.x(), y.y(), 0.15f);
    glVertex3f(zero.x(), zero.y(), 0.15f);
    glEnd();

    glLineWidth(1.0f);
}

// On mouse wheel activity, zoom in (was implemented early to debug OpenGL implementation)
void OpenGL::wheelEvent(QWheelEvent *event)
{
    float camera_zoom;
    if (event->angleDelta().y() < 0)
        camera_zoom = this->_camera->zoom(0.25f);
    else
        camera_zoom = this->_camera->zoom(-0.25f);
    this->_control_cam_zoom->setSliderPosition((int)(4.0f * camera_zoom));
    this->update();
}

// When the OpenGL widget is clicked, enable dragging and set the starting point
void OpenGL::mousePressEvent(QMouseEvent *event)
{
    // if (event->button() == Qt::LeftButton)
    this->_prev = event->pos();
}

// When the mouse moves. Mouse tracking is off = only called if clicked (dragging)
void OpenGL::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() != Qt::LeftButton)
        return;
    // Get modifiers (shift key)
    Qt::KeyboardModifiers modifiers = event->modifiers();

    // Get and update the relative movement
    QPoint delta = this->_prev - event->pos();
    this->_prev = event->pos();

    // If shift is clicked rotated the sun instead
    if (modifiers.testFlag(Qt::ShiftModifier))
    {
        float sun_rotation_x = this->_light->rotateX(delta.y());
        float sun_rotation_y = this->_light->rotateY(delta.x());
        this->_control_sun_rotation_x->setSliderPosition((int)sun_rotation_x);
        this->_control_sun_rotation_y->setSliderPosition(360 - (int)sun_rotation_y);
    }
    // Otherwise default to the camera only
    else
    {
        float cam_rotation_x = this->_camera->rotateX(-delta.y());
        float cam_rotation_y = this->_camera->rotateY(-delta.x());
        this->_control_cam_rotation_x->setSliderPosition((int)cam_rotation_x);
        this->_control_cam_rotation_y->setSliderPosition((int)cam_rotation_y);
    }
    // Update the OpenGL data
    update();
}

// When the sun widgets are changed
void OpenGL::sunRotationX(int value)
{
    this->_light->setRotationX((float)value);
    this->update();
}
void OpenGL::sunRotationY(int value)
{
    this->_light->setRotationY(360.0f - (float)value);
    this->update();
}

// When the camera widgets are changed
void OpenGL::camRotationX(int value)
{
    this->_camera->setRotationX((float)value);
    this->update();
}
void OpenGL::camRotationY(int value)
{
    this->_camera->setRotationY((float)value);
    this->update();
}
void OpenGL::camZoom(int value)
{
    this->_camera->setZoom(((float)value) / 4.0f);
    this->update();
}

// When the nodeeditor has new updated textures
void OpenGL::nodeeditorOutputUpdated(QImage normal_map, QImage height_map)
{
    this->_terrain->setHeightMap(height_map);
    this->_terrain->setNormalMap(normal_map);
    this->update();
}
