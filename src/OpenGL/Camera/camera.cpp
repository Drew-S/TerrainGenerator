#include "camera.h"

// Return the calculated Perspective * View matrix (camera)
QMatrix4x4 Camera::matrix()
{
    // Get the view matrix
    QMatrix4x4 view;
    // Translated (zoom)
    view.translate(QVector3D(0.0f, 0.0f, -this->_zoom));
    // Rotated (above terrain, then turntable)
    view.rotate(this->_rotation_x, QVector3D(1.0f, 0.0f, 0.0f));
    view.rotate(this->_rotation_y, QVector3D(0.0f, 1.0f, 0.0f));

    QMatrix4x4 projection;
    // Generate the perspective matrix
    // TODO: include a toggle for orthographic mode
    projection.perspective(45.0f, this->_projection, 0.1f, 100.0f);

    // Return camera matrix
    return projection * view;
}

// Called when the widget resizes to adjust perspective values
void Camera::resize(int w, int h)
{
    this->_projection = w / float(h);
}