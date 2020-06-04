#pragma once

#include <QMatrix4x4>
#include <QObject>
#include <QVector3D>

/**
 * Camera
 * 
 * Houses the projection matrix and the view matrix. Used to changed the
 * values for the generation of said matrices. Can return the current rotations
 * of the camera, the zoom, and the position of the camera in world space.
 */
class Camera : public QObject
{
    Q_OBJECT
public:
    // Return the generated combined projection and view matrix
    QMatrix4x4 matrix();

    // Return the rotation of the matrix only
    QMatrix4x4 rotationMatrix();

    // Return the projection matrix only
    QMatrix4x4 projectionMatrix();

    // Returns the generated camera world position
    QVector3D position();

    // Rotate the camera (turntable) around the terrain
    float rotateY(float value);

    // Rotate the camera over the terrain
    float rotateX(float value);

    // Zoom the camera into/out of the terrain
    float zoom(float value);
    float zoom();

    // Set the rotation (turntable) around the terrain
    float setRotationY(float value);

    // Set the rotation over the terrain
    float setRotationX(float value);

    // Set the zoom level into the terrain
    float setZoom(float value);

    // Resize call, when widget size change recalculate the projection matrix
    void resize(int w, int h);

    // Get the current rotation values for the different axis
    float rotationX();
    float rotationY();

private:
    // Get the view matrix (no projection)
    QMatrix4x4 _matrix();

    // Applies rotation limits above the terrain
    void _clampRotationX();
    // Rotation over the terrain
    float _rotation_x = 0.0f;
    // Rotation around the terrain (turntable)
    float _rotation_y = 0.0f;
    // Zoom distance from the terrain
    float _zoom = 1.0f;

    // Perspective camera horizontal viewing angle ratio
    float _projection = 1.0f;
};
