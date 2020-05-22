#include "camera.h"

#define Q_BETWEEN(low, v, hi) Q_ASSERT(low <= v && v <= hi)

Camera::Camera() {}
Camera::~Camera() {}

// Return the calculated View matrix, used for getting the position of the camera
// and for getting the combined projection and view matrix
QMatrix4x4 Camera::_matrix()
{
    QMatrix4x4 view;
    // Translate (zoom out/in)
    // TODO: Add adjustment for panning up and down y-axis
    view.translate(QVector3D(0.0f, -0.3333f, -this->_zoom));
    // Rotate over the terrain
    view.rotate(this->_rotation_x, QVector3D(1.0f, 0.0f, 0.0f));
    // Rotate around the terrain (turntable)
    view.rotate(this->_rotation_y, QVector3D(0.0f, 1.0f, 0.0f));

    return view;
}

// Return the calculated camera matrix, the combined projection and view matrix
QMatrix4x4 Camera::matrix()
{
    // Return camera matrix (combined view and projection)
    return this->projectionMatrix() * this->_matrix();
}

// Get the cameras rotation matrix (rotation effects only, used for axis indication)
QMatrix4x4 Camera::rotationMatrix()
{
    QMatrix4x4 view;
    // Rotate over the terrain
    view.rotate(this->_rotation_x, QVector3D(1.0f, 0.0f, 0.0f));
    // Rotate around the terrain (turntable)
    view.rotate(this->_rotation_y, QVector3D(0.0f, 1.0f, 0.0f));

    return view;
}

// Get the projection matrix
QMatrix4x4 Camera::projectionMatrix()
{
    QMatrix4x4 projection;
    // Generate the perspective matrix
    // TODO: include a toggle for orthographic mode
    projection.perspective(45.0f, this->_projection, 0.01f, 100.0f);
    return projection;
}

// Returns the position of the camera in the world space
QVector3D Camera::position()
{
    // Get the inversed view matrix
    QMatrix4x4 view = this->_matrix().inverted();

    // Apply the matrix to the 0 vector to get the camera position
    return view * QVector3D();
}

// Update the projection ratio used in the projection matrix for the camera matrix
void Camera::resize(int w, int h)
{
    this->_projection = w / float(h);
}

// Rotate the camera (turntable) around the terrain
float Camera::rotateY(float value)
{
    this->_rotation_y += value;
    return this->_rotation_y;
}

// Rotate the camera over the terrain
float Camera::rotateX(float value)
{
    this->_rotation_x += value;
    this->_clampRotationX(); // Limits rotation
    return this->_rotation_x;
}

// Zoom the camera into/out of the terrain
float Camera::zoom(float value)
{
    this->_zoom += value;
    if (this->_zoom < 1.0f)
        this->_zoom = 1.0f;

    if (this->_zoom > 20.0f)
        this->_zoom = 20.0f;
    return this->_zoom;
}
float Camera::zoom()
{
    Q_BETWEEN(1.0f, this->_zoom, 20.0f);
    return this->_zoom;
}

// Set the rotation (turntable) around the terrain
float Camera::setRotationY(float value)
{
    this->_rotation_y = value;
    return this->_rotation_y;
}

// Set the rotation over the terrain
float Camera::setRotationX(float value)
{
    this->_rotation_x = value;
    this->_clampRotationX(); // limits rotation
    Q_BETWEEN(0.0f, this->_rotation_x, 90.0f);
    return this->_rotation_x;
}

// Set the zoom level into the terrain
float Camera::setZoom(float value)
{
    this->_zoom = value;
    return this->_zoom;
}

// Get rotation values
float Camera::rotationX()
{
    return this->_rotation_x;
}
float Camera::rotationY()
{
    return this->_rotation_y;
}

// Applies rotation limits above the terrain
void Camera::_clampRotationX()
{
    if (this->_rotation_x < 0.0f)
        this->_rotation_x = 0.0f;
    if (this->_rotation_x > 90.0f)
        this->_rotation_x = 90.0f;
}
