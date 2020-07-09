#include "camera.h"

#ifdef _WIN32
#include <windows.h>
#endif

#define Q_BETWEEN(low, v, hi) Q_ASSERT(low <= v && v <= hi)

/**
 * _matrix
 * 
 * Returns the calculated view matrix only.
 * 
 * @returns QMatrix4x4 : The view matrix.
 */
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

/**
 * matrix
 * 
 * Returns the combined view and projection matrix.
 * 
 * @returns QMatrix4x4 : The combined view and projection matrices.
 */
QMatrix4x4 Camera::matrix()
{
    // Return camera matrix (combined view and projection)
    return this->projectionMatrix() * this->_matrix();
}

/**
 * rotationMatrix
 * 
 * Returns the rotation matrix, the rotations along the x- and y-axis and not
 * the projections or the view. This is used for the decals that should only be
 * rotated on screen as a flat image and not in 3D space.
 * 
 * @returns QMatrix4x4 : The rotation matrix.
 */
QMatrix4x4 Camera::rotationMatrix()
{
    QMatrix4x4 view;
    // Rotate over the terrain
    view.rotate(this->_rotation_x, QVector3D(1.0f, 0.0f, 0.0f));
    // Rotate around the terrain (turntable)
    view.rotate(this->_rotation_y, QVector3D(0.0f, 1.0f, 0.0f));

    return view;
}

/**
 * projectionMatrix
 * 
 * Returns the projection matrix only.
 * 
 * @return QMatrix4x4 : The projection matrix.
 */
QMatrix4x4 Camera::projectionMatrix()
{
    QMatrix4x4 projection;
    // Generate the perspective matrix
    // TODO: include a toggle for orthographic mode
    projection.perspective(45.0f, this->_projection, 0.01f, 100.0f);
    return projection;
}

/**
 * position
 * 
 * Returns the position of the camera in 3D world space.
 * 
 * @returns QVector3D : The position of the camera.
 */
QVector3D Camera::position()
{
    // Get the inversed view matrix
    QMatrix4x4 view = this->_matrix().inverted();

    // Apply the matrix to the 0 vector to get the camera position
    return view * QVector3D();
}

// Update the projection ratio used in the projection matrix for the camera matrix
/**
 * resize
 * 
 * Resize the camera and recalculates the projection ration used for the
 * projection matrix.
 * 
 * @param int w : The width of the viewport.
 * @param int h : The height of the viewport.
 */
void Camera::resize(int w, int h)
{
    this->_projection = w / float(h);
}

/**
 * rotateY
 * 
 * Rotates the camera about the terrain (turntable style). The new current
 * rotation is returned (rotation is clamped between 0 and 359).
 * 
 * @param float value : The value to rotate by.
 * 
 * @returns float : The new rotation value.
 */
float Camera::rotateY(float value)
{
    this->_rotation_y += value;
    return this->_rotation_y;
}

/**
 * rotateX
 * 
 * Rotates the camera over the terrain. The new current rotation is returned
 * (rotation is clamped between 0 and 89).
 * 
 * @param float value : The value to rotate by.
 * 
 * @returns float : The new rotation value.
 */
float Camera::rotateX(float value)
{
    this->_rotation_x += value;
    this->_clampRotationX(); // Limits rotation
    return this->_rotation_x;
}

/**
 * zoom
 * 
 * Zooms into and out of the terrain along a track from the current camera's
 * position and the world origin. The zoom is clamped between 1 and 20.
 * 
 * @param float value : The value to zoom by.
 * 
 * @returns float : The new zoom value.
 */
float Camera::zoom(float value)
{
    this->_zoom += value;
    if (this->_zoom < 1.0f)
        this->_zoom = 1.0f;

    if (this->_zoom > 20.0f)
        this->_zoom = 20.0f;
    return this->_zoom;
}

/**
 * zoom
 * 
 * Get the current zoom value.
 * 
 * @returns float : The current zoom value.
 */
float Camera::zoom()
{
    Q_BETWEEN(1.0f, this->_zoom, 20.0f);
    return this->_zoom;
}

/**
 * setRotationY
 * 
 * Sets the rotation y value, overriding the previous value rather than adding
 * onto it.
 * 
 * @param float value : The value to set the rotation to.
 * 
 * @returns float : The new value.
 */
float Camera::setRotationY(float value)
{
    this->_rotation_y = value;
    return this->_rotation_y;
}

/**
 * setRotationX
 * 
 * Sets the rotation x value, overriding the previous value rather than adding
 * onto it. The value is clamped between 0 and 90.
 * 
 * @param float value : The value to set the rotation to.
 * 
 * @returns float : The new value.
 */
float Camera::setRotationX(float value)
{
    this->_rotation_x = value;
    this->_clampRotationX(); // limits rotation
    Q_BETWEEN(0.0f, this->_rotation_x, 90.0f);
    return this->_rotation_x;
}

/**
 * setZoom
 * 
 * Sets the zoom value overriding the current value rather than adding to it.
 * 
 * @param float value : The value to set the zoom to.
 * 
 * @returns float : The new zoom value.
 */
float Camera::setZoom(float value)
{
    this->_zoom = value;
    return this->_zoom;
}

/**
 * rotationX
 * 
 * Returns the current rotation of the x value.
 * 
 * @returns float : The current rotation.
 */
float Camera::rotationX()
{
    return this->_rotation_x;
}

/**
 * rotationY
 * 
 * Returns the current rotation of the y value.
 * 
 * @returns float : The current rotation.
 */
float Camera::rotationY()
{
    return this->_rotation_y;
}

/**
 * _clampRotationX
 * 
 * Clamps the rotation over the the terrain between 0 and 90 degrees.
 */
void Camera::_clampRotationX()
{
    if (this->_rotation_x < 0.0f)
        this->_rotation_x = 0.0f;
    if (this->_rotation_x > 90.0f)
        this->_rotation_x = 90.0f;
}
