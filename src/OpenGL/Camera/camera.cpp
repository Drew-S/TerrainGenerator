#include "camera.h"

// Return the calculated View matrix, used for getting the position of the camera
// and for getting the combined projection and view matrix
QMatrix4x4 Camera::_matrix()
{
    QMatrix4x4 view;
    // Translate (zoom out/in)
    view.translate(QVector3D(0.0f, 0.0f, -this->_zoom));
    // Rotate over the terrain
    view.rotate(this->_rotation_x, QVector3D(1.0f, 0.0f, 0.0f));
    // Rotate around the terrain (turntable)
    view.rotate(this->_rotation_y, QVector3D(0.0f, 1.0f, 0.0f));

    return view;
}

// Return the calculated camera matrix, the combined projection and view matrix
QMatrix4x4 Camera::matrix()
{
    // Get the view matrix
    QMatrix4x4 view = this->_matrix();

    QMatrix4x4 projection;
    // Generate the perspective matrix
    // TODO: include a toggle for orthographic mode
    projection.perspective(45.0f, this->_projection, 0.01f, 100.0f);

    // Return camera matrix (combined view and projection)
    return projection * this->_matrix();
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