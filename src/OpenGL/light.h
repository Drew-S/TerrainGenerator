#pragma once

#include <QVector>
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

// Manages the lights position for use in OpenGL shaders
class Light
{
public:
    Light();
    ~Light();

    // Returns the position of the light in world space
    QVector3D position();

    // Returns the intensity of the light
    float intensity();

    // Returns the color of the light as QVector3D for easy use in OpenGL shaders
    QVector3D color();

    // Rotates the light around the terrain (turntable)
    float rotateY(float value);

    // Rotates the light over the terrain
    float rotateX(float value);

    // Sets the rotation around the terrain (turntable)
    float setRotationY(float value);

    // Sets the rotation over the terrain
    float setRotationX(float value);

    // Set the intensity of the light
    void setIntensity(float value);

    // Set the color of the light
    void setColor(QVector3D color);

    // Draw the sun onto the screen
    void paintGL(QMatrix4x4 camera);

private:
    // Limits the rotation of the camera over the terrain
    void _clampRotationX();

    // Rotation over the terrain 0 -> 90
    float _rotation_x = 0.0f;

    // Rotation around the terrain (turntable)
    float _rotation_y = 0.0f;

    // Lights intensity
    float _intensity = 1.0f;

    // Lights color (defaults to white)
    QVector3D _color{1.0f, 1.0f, 1.0f};
};
