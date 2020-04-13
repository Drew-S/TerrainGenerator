#pragma once

#include <QMatrix4x4>
#include <QVector3D>

static const QVector3D UP(0.0f, 1.0f, 0.0f);
static const QVector3D ZERO(0.0f, 0.0f, 0.0f);

class Camera
{
public:
    Camera(){};
    ~Camera(){};

    // Return the generated lookat matrix
    QMatrix4x4 matrix();

    // TODO: implement rotation and zoom limits

    // Rotate the camera (turntable) around the terrain
    void rotateY(float v) { this->_rotation_y += v; };

    // Rotate the camera over the terrain
    void rotateX(float v) { this->_rotation_x += v; };

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
    void setRotationY(float v) { this->_rotation_y = v; };

    // Set the rotation over the terrain
    void setRotationX(float v) { this->_rotation_x = v; };

    // Set the zoom level into the terrain
    void setZoom(float v) { this->_zoom = v; }

    // Resize call, when widget size change recalculate the projection matrix
    void resize(int w, int h);

private:
    // Rotation over the terrain
    float _rotation_x = 0.0f;
    // Rotation around the terrain (turntable)
    float _rotation_y = 0.0f;
    // Zoom distance from the terrain
    float _zoom = 1.0f;

    // Perspective camera horizontal viewing angle ratio
    float _projection = 1.0f;
};