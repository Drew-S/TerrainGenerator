#include "light.h"

#include <QMatrix>
#include <QOpenGLBuffer>
#include <QOpenGLShader>

#include <GL/gl.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define Q_BETWEEN(low, v, hi) Q_ASSERT(low <= v && v <= hi)

/**
 * position
 * 
 * Return the position of the sun in the world space.
 * 
 * @returns QVector3D : The position of the sun.
 */
QVector3D Light::position()
{
    // Apply rotational transformations
    QMatrix4x4 matrix;
    matrix.rotate(this->_rotation_y, QVector3D(0.0f, 1.0f, 0.0f));
    matrix.rotate(this->_rotation_x, QVector3D(-1.0f, 0.0f, 0.0f));

    // Apply rotation to a placed light
    return matrix * QVector3D(0.0f, 0.0f, 2.0f);
}

/**
 * paintGL
 * 
 * Draws the sun icon and and direction lines onto the opengl widget.
 * 
 * @param QMatrix4x4 camera : The projection and view matrix of the camera.
 * @param float width : The width of the viewport.
 * @param float height : The height of the viewport.
 */
void Light::paintGL(QMatrix4x4 camera, float width, float height)
{
    QVector3D pos = this->position();

    QMatrix4x4 rotate;
    rotate.rotate(45.0f, 0.0f, 0.0f, 1.0f);

    // Draw a point sun
    glPointSize(5.0f);

    // The origin relative to the camera transformation
    QVector3D zero = camera * QVector3D();

    // The light location on the y=0 plane
    QVector3D plane = camera * QVector3D(pos.x(), 0.0f, pos.z());

    // The position of the light relative to the camera transformation
    pos = camera * pos;

    // Size in pixels
    float i_size = 80.0f;

    QVector3D min(0.3f, 0.0f, 0.0f);
    QVector3D max(0.8f, 0.0f, 0.0f);

    QMatrix transform;
    transform = transform * QMatrix(i_size / 2.0f, 0.0f, 0.0f, i_size / 2.0f, 0.0f, 0.0f);
    transform = transform * QMatrix(1.0f / width, 0.0f, 0.0f, 1.0f / height, 0.0f, 0.0f);

    // TODO: Include a toggle to disable this in the UI

    // Draw the sun point
    glBegin(GL_POINTS);
    glColor3f(this->_color.x(), this->_color.y(), this->_color.z());
    glVertex3f(pos.x(), pos.y(), pos.z());
    glEnd();

    glBegin(GL_LINES);
    glColor3f(this->_color.x(), this->_color.y(), this->_color.z());
    for (int i = 0; i < 8; i++)
    {
        min = rotate * min;
        max = rotate * max;
        QVector3D min_prime = pos + (transform * min);
        QVector3D max_prime = pos + (transform * max);
        glVertex3f(min_prime.x(), min_prime.y(), min_prime.z());
        glVertex3f(max_prime.x(), max_prime.y(), max_prime.z());
    }
    glEnd();

    // Draw lines to display the lights direction to the terrain
    glPushAttrib(GL_ENABLE_BIT);

    // Draw a dashed lines
    glLineStipple(4, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    glColor3f(this->_color.x(), this->_color.y(), this->_color.z());

    // Draw from the sun position to the origin
    glVertex3f(pos.x(), pos.y(), pos.z());
    glVertex3f(zero.x(), zero.y(), zero.z());

    // Draw from the sun position on the y-plane to the origin
    glVertex3f(plane.x(), plane.y(), plane.z());
    glVertex3f(zero.x(), zero.y(), zero.z());

    // Draw from the sun position to the y-plane
    glVertex3f(pos.x(), pos.y(), pos.z());
    glVertex3f(plane.x(), plane.y(), plane.z());
    glEnd();

    glPopAttrib();
}

/**
 * intensity
 * 
 * Return the intensity of the sun.
 * 
 * @returns float : The intensity of the light.
 */
float Light::intensity()
{
    return this->_intensity;
}

/**
 * color
 * 
 * Returns the colour of the light in vector form.
 * 
 * @returns QVector3D : The light colour.
 */
QVector3D Light::color()
{
    return this->_color;
}

/**
 * rotateY
 * 
 * Rotates the light around the terrain (turntable).
 * 
 * @param float value : The value to rotate the light by.
 * 
 * @returns float : The new rotation value.
 */
float Light::rotateY(float value)
{
    this->_rotation_y += value;
    return this->_rotation_y;
}

/**
 * rotateX
 * 
 * Rotates the light above the terrain. Rotation is clamped between 0 and 90
 * degrees.
 * 
 * @param float value : The value to rotate the light by.
 * 
 * @returns float : The new rotation value.
 */
float Light::rotateX(float value)
{
    this->_rotation_x += value;
    this->_clampRotationX(); // Limits the rotation
    Q_BETWEEN(0.0f, this->_rotation_x, 90.0f);
    return this->_rotation_x;
}

/**
 * setRotationY
 * 
 * Sets the rotation around the terrain. This overrides the previous value.
 * Returns the updated rotation value.
 * 
 * @param float value : The new value to override with.
 * 
 * @returns float : The new rotation value.
 */
float Light::setRotationY(float value)
{
    this->_rotation_y = value;
    return this->_rotation_y;
}

/**
 * setRotationX
 * 
 * Sets the rotation over the terrain. This overrides the previous value.
 * Returns the updated rotation value. Rotation is clamped between 0 and 90
 * degrees.
 * 
 * @param float value : The new value to override with.
 * 
 * @returns float : The new rotation value.
 */
float Light::setRotationX(float value)
{
    this->_rotation_x = value;
    this->_clampRotationX(); // Limits the rotation
    return this->_rotation_x;
}

/**
 * setIntensity
 * 
 * Sets the light intensity.
 * 
 * @param float value : The value to set the sun intensity.
 */
void Light::setIntensity(float value)
{
    this->_intensity = value;
}

/**
 * setColor
 * 
 * Sets the lights colour.
 * 
 * @param QVector3D color : The color to set the light to.
 */
void Light::setColor(QVector3D color)
{
    this->_color = color;
}

/**
 * _clampRotationX
 * 
 * Clamps the rotation over the terrain between 0 and 90 degrees.
 */
void Light::_clampRotationX()
{
    if (this->_rotation_x < 0.0)
        this->_rotation_x = 0.0;
    if (this->_rotation_x > 90.0)
        this->_rotation_x = 90.0;
}
