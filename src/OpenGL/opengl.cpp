#include "opengl.h"

#include <QDebug>
#include <QMatrix>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QPoint>
#include <QPointF>
#include <QSurfaceFormat>

#include <GL/gl.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "ui_OpenGLControls.h"

/**
 * OpenGL
 * 
 * Create the opengl widget, setup the terrain, light, camera, and other
 * information.
 * 
 * @param QWidget* parent : Set the parent widget.
 */
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

    // TODO: Use _ui private and remove this->_control*
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
    QObject::connect(this->_control_cam_rotation_x,
                     &QSlider::valueChanged,
                     this,
                     &OpenGL::camRotationX);

    QObject::connect(this->_control_cam_rotation_y,
                     &QSlider::valueChanged,
                     this,
                     &OpenGL::camRotationY);

    QObject::connect(this->_control_cam_zoom,
                     &QSlider::valueChanged,
                     this,
                     &OpenGL::camZoom);

    // Attach listeners for the sun controls
    QObject::connect(this->_control_sun_rotation_x,
                     &QSlider::valueChanged,
                     this,
                     &OpenGL::sunRotationX);

    QObject::connect(this->_control_sun_rotation_y,
                     &QSlider::valueChanged,
                     this,
                     &OpenGL::sunRotationY);
}

/**
 * ~OpenGL
 * 
 * Deletes the pointers to the other values. Destroys the widget.
 */
OpenGL::~OpenGL()
{
    Q_CHECK_PTR(this->_terrain);
    Q_CHECK_PTR(this->_camera);
    Q_CHECK_PTR(this->_light);
    delete this->_terrain;
    delete this->_camera;
    delete this->_light;
}

/**
 * setTerrainDrawLines
 * 
 * Set whether or not to draw terrain vertex lines.
 * 
 * @param bool lines : Whether or not to draw the lines
 */
void OpenGL::setTerrainDrawLines(bool lines)
{
    Q_CHECK_PTR(this->_terrain);
    this->_terrain->setDrawLines(lines);
    this->update();
}

/**
 * setTerrainColor
 * 
 * Sets the colour of the terrain.
 * 
 * @param QColo color : The colour of the terrain.
 */
void OpenGL::setTerrainColor(QColor color)
{
    Q_CHECK_PTR(this->_terrain);
    this->_terrain->setTerrainColor(color);
    this->update();
}

/**
 * setTerrainLineColor
 * 
 * Set the color of the terrain lines.
 * 
 * @param QColor color : The colour of the terrain lines.
 */
void OpenGL::setTerrainLineColor(QColor color)
{
    Q_CHECK_PTR(this->_terrain);
    this->_terrain->setLineColor(color);
    this->update();
}

/**
 * setTerrainMeshResolution
 * 
 * Sets the resolution of the terrain mesh.
 * 
 * @param int resolution : The resolution of the mesh.
 */
void OpenGL::setTerrainMeshResolution(int resolution)
{
    Q_CHECK_PTR(this->_terrain);
    this->_terrain->setResolution(resolution);
    this->update();
}

/**
 * setSkyColor
 * 
 * Set the background colour of the sky, also used for the ambient colour.
 * 
 * @param QColor color : The colour of the sky.
 */
void OpenGL::setSkyColor(QColor color)
{
    this->_ambient = QVector3D(color.redF(), color.greenF(), color.blueF());
    this->update();
}

/**
 * setLightColor
 * 
 * Sets the colour of the sun.
 * 
 * @param QColor color : The colour of the sun.
 */
void OpenGL::setLightColor(QColor color)
{
    Q_CHECK_PTR(this->_light);
    this->_light->setColor(QVector3D(color.redF(), color.greenF(), color.blueF()));
    this->update();
}

/**
 * terrainDrawLines
 * 
 * Get whether we are drawing lines on the terrain or not.
 * 
 * @returns bool : Whether the line are being drawn.
 */
bool OpenGL::terrainDrawLines()
{
    Q_CHECK_PTR(this->_terrain);
    return this->_terrain->drawLines();
}

/**
 * terrainColor
 * 
 * Get the current terrain colour.
 * 
 * @returns QColor : The current colour of the terrain.
 */
QColor OpenGL::terrainColor()
{
    Q_CHECK_PTR(this->_terrain);
    return this->_terrain->terrainColor();
}

/**
 * terrainLineColor
 * 
 * Get the colour of the terrain lines.
 * 
 * @returns QColor : The current terrain line colour.
 */
QColor OpenGL::terrainLineColor()
{
    Q_CHECK_PTR(this->_terrain);
    return this->_terrain->lineColor();
}

/**
 * skyColor
 * 
 * Get the current sky colour.
 * 
 * @returns QColor : The current sky colour.
 */
QColor OpenGL::skyColor()
{
    return QColor::fromRgbF(this->_ambient.x(), this->_ambient.y(), this->_ambient.z());
}

/**
 * lightColor
 * 
 * Get the current sun colour.
 * 
 * @returns QColor : The current light colour.
 */
QColor OpenGL::lightColor()
{
    Q_CHECK_PTR(this->_light);
    QVector3D vec = this->_light->color();
    return QColor::fromRgbF(vec.x(), vec.y(), vec.z());
}

/**
 * initializeGL
 * 
 * Initializes the OpenGL functions, the parameters, the terrain, the camera,
 * etc.
 */
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
    f->glClearColor(this->_ambient.x(), this->_ambient.y(), this->_ambient.z(), 1.0f);

    Q_CHECK_PTR(this->_terrain);
    // Initialize terrain's OpenGL
    this->_terrain->initializeGL();
}

/**
 * resizeGL
 * 
 * Updates the widget resolution, updates the gizmos, and updates the camera
 * values.
 * 
 * @param int w : The width of the widget.
 * @param iht h : The height of the widget.
 */
void OpenGL::resizeGL(int w, int h)
{
    Q_CHECK_PTR(this->_terrain);
    Q_CHECK_PTR(this->_controls);
    this->_camera->resize(w, h);
    this->_controls->setGeometry(0, -10, w, h);
}

/**
 * paintGL
 * 
 * Updates the viewport, the gizmos, terrain, etc.
 */
void OpenGL::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    // Clear screen
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set sky color
    f->glClearColor(this->_ambient.x(), this->_ambient.y(), this->_ambient.z(), 1.0f);

    Q_CHECK_PTR(this->_camera);
    Q_CHECK_PTR(this->_light);
    Q_CHECK_PTR(this->_terrain);
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
        light_intensity,
        this->_ambient);

    // Current widget width and height
    float width = (float)this->size().width();
    float height = (float)this->size().height();

    // Draw the sun graphic
    this->_light->paintGL(camera_matrix, width, height);

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

/**
 * wheelEvent
 * 
 * When the wheel is scrolled we adjust the zoom value of the camera.
 * 
 * @param QWheelEvent* event : The mouse scroll wheel event.
 */
void OpenGL::wheelEvent(QWheelEvent *event)
{
    Q_CHECK_PTR(this->_camera);
    Q_CHECK_PTR(this->_control_cam_zoom);
    float camera_zoom;
    if (event->angleDelta().y() < 0)
        camera_zoom = this->_camera->zoom(0.25f);
    else
        camera_zoom = this->_camera->zoom(-0.25f);
    this->_control_cam_zoom->setSliderPosition((int)(4.0f * camera_zoom));
    this->update();
}

/**
 * mousePressEvent
 * 
 * On mouse press we save the previous position for relative mouse movement used
 * for rotation in the move event.
 * 
 * @param QMouseEvent* event : The mouse press event.
 */
void OpenGL::mousePressEvent(QMouseEvent *event)
{
    this->_prev = event->pos();
}

/**
 * mouseMoveEvent
 * 
 * When the mouse is moved and the correct buttons are pressed we update the
 * camera or the suns rotation as the mouse moves.
 * 
 * @param QMouseEvent* event : The mouse move event.
 */
void OpenGL::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() != Qt::LeftButton)
        return;
    // Get modifiers (shift key)
    Qt::KeyboardModifiers modifiers = event->modifiers();

    // Get and update the relative movement
    QPoint delta = this->_prev - event->pos();
    this->_prev = event->pos();

    Q_CHECK_PTR(this->_light);
    Q_CHECK_PTR(this->_control_sun_rotation_x);
    Q_CHECK_PTR(this->_control_sun_rotation_y);

    Q_CHECK_PTR(this->_camera);
    Q_CHECK_PTR(this->_control_cam_rotation_x);
    Q_CHECK_PTR(this->_control_cam_rotation_y);

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
    this->update();
}

/**
 * sunRotationX @slot
 * 
 * Update the suns rotation along the x-axis from the UI.
 * 
 * @param int value : The new rotation value.
 */
void OpenGL::sunRotationX(int value)
{
    Q_CHECK_PTR(this->_light);
    this->_light->setRotationX((float)value);
    this->update();
}

/**
 * sunRotationY @slot
 * 
 * Update the suns rotation along the y-axis from the UI.
 * 
 * @param int value : The new rotation value.
 */
void OpenGL::sunRotationY(int value)
{
    Q_CHECK_PTR(this->_light);
    this->_light->setRotationY(360.0f - (float)value);
    this->update();
}

/**
 * camRotationX @slot
 * 
 * Update the camera rotation along the x-axis from the UI.
 * 
 * @param int value : The new rotation value.
 */
void OpenGL::camRotationX(int value)
{
    Q_CHECK_PTR(this->_camera);
    this->_camera->setRotationX((float)value);
    this->update();
}

/**
 * camRotationY @slot
 * 
 * Update the camera rotation along the y-axis from the UI.
 * 
 * @param int value : The new rotation value.
 */
void OpenGL::camRotationY(int value)
{
    Q_CHECK_PTR(this->_camera);
    this->_camera->setRotationY((float)value);
    this->update();
}
/**
 * camZoom @slot
 * 
 * Update the camera zoom value from UI.
 * 
 * @param int value : The new zoom value.
 */
void OpenGL::camZoom(int value)
{
    Q_CHECK_PTR(this->_camera);
    this->_camera->setZoom(((float)value) / 4.0f);
    this->update();
}

// When the nodeeditor has new updated textures
/**
 * nodeeditorOutputUpdated @slot
 * 
 * Called when the nodeeditor has updated normal/height maps from the dataflow
 * diagram. Used to update the scene.
 * 
 * @param QImage normal_map : The new normal map.
 * @param QImage height_map : The new height map.
 * @param QImage albedo_map : The new albedo map.
 */
void OpenGL::nodeeditorOutputUpdated(QImage normal_map,
                                     QImage height_map,
                                     QImage albedo_map)
{
    Q_CHECK_PTR(this->_terrain);
    this->_terrain->setHeightMap(height_map);
    this->_terrain->setNormalMap(normal_map);
    this->_terrain->setAlbedoMap(albedo_map);
    this->update();
}
