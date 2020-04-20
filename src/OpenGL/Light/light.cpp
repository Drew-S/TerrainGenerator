#include "light.h"

#include <QOpenGLShader>
#include <QOpenGLBuffer>

#include <GL/gl.h>

// TODO: Add control to set light color
#define LIGHT_COLOR 1.0f, 1.0f, 1.0f

// Get the position of the light in world coordinates
QVector3D Light::position()
{
    // Apply rotational transformations
    QMatrix4x4 matrix;
    matrix.rotate(this->_rotation_y, QVector3D(0.0f, 1.0f, 0.0f));
    matrix.rotate(this->_rotation_x, QVector3D(-1.0f, 0.0f, 0.0f));

    // Apply rotation to a placed light
    return matrix * QVector3D(0.0f, 0.0f, 2.0f);
}

// Used to draw the sun into the world
// TODO: Improve the sun icon appearance
void Light::paintGL(QMatrix4x4 camera)
{
    QVector3D pos = this->position();

    // Draw a point sun
    glPointSize(5.0f);

    // The origin relative to the camera transformation
    QVector3D zero = camera * QVector3D();

    // The light location on the y=0 plane
    QVector3D plane = camera * QVector3D(pos.x(), 0.0f, pos.z());

    // The position of the light relative to the camera transformation
    pos = camera * pos;

    // TODO: Include a toggle to disable this in the UI

    // Draw the sun point
    glBegin(GL_POINTS);
    glColor3f(LIGHT_COLOR);
    glVertex3f(pos.x(), pos.y(), pos.z());
    glEnd();

    // Draw lines to display the lights direction to the terrain
    glPushAttrib(GL_ENABLE_BIT);

    // Draw a dashed lines
    glLineStipple(4, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    glColor3f(LIGHT_COLOR);

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