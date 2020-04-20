#pragma once

#include <QMatrix4x4>
#include <QVector3D>

// TODO: Move all function implementations to *.cpp files globally (all files)

// Camera class manages the QMatrix4x4 projection and view matrix and the QVector3D position for use
// in the OpenGL shaders.
class Camera
{
public:
    Camera(){};
    ~Camera(){};

    // Return the generated combined projection and view matrix
    QMatrix4x4 matrix();

    // Returns the generated camera world position
    QVector3D position();

    // Rotate the camera (turntable) around the terrain
    void rotateY(float v) { this->_rotation_y += v; };

    // Rotate the camera over the terrain
    void rotateX(float v)
    {
        this->_rotation_x += v;
        this->_clampRotationX(); // Limits rotation
    };

    // Zoom the camera into/out of the terrain
    void zoom(float v)
    {
        this->_zoom += v;
        if (this->_zoom < 1.0f)
            this->_zoom = 1.0f;

        if (this->_zoom > 20.0f)
            this->_zoom = 20.0f;
    };

    // Set the rotation (turntable) around the terrain
    void setRotationZ(float v) { this->_rotation_y = v; };

    // Set the rotation over the terrain
    void setRotationX(float v)
    {
        this->_rotation_x = v;
        this->_clampRotationX(); // limits rotation
    };

    // Set the zoom level into the terrain
    void setZoom(float v) { this->_zoom = v; }

    // Resize call, when widget size change recalculate the projection matrix
    void resize(int w, int h);

private:
    // Get the view matrix (no projection)
    QMatrix4x4 _matrix();

    // Applies rotation limits above the terrain
    void _clampRotationX()
    {
        if (this->_rotation_x < 0.0)
            this->_rotation_x = 0.0;
        if (this->_rotation_x > 90.0)
            this->_rotation_x = 90.0;
    };
    // Rotation over the terrain
    float _rotation_x = 0.0f;
    // Rotation around the terrain (turntable)
    float _rotation_y = 0.0f;
    // Zoom distance from the terrain
    float _zoom = 1.0f;

    // Perspective camera horizontal viewing angle ratio
    float _projection = 1.0f;
};