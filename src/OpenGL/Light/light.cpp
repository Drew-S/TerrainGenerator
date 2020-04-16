#include "light.h"

#include <QMatrix4x4>

Light::Light(float rotation_x, float rotation_y, float intensity, QColor color)
    : _rotation_x(rotation_x), _rotation_y(rotation_y), _intensity(intensity), _color(color) {}

QVector3D Light::direction()
{
    QMatrix4x4 matrix;
    matrix.rotate(this->_rotation_x, QVector3D(1.0f, 0.0f, 0.0f));
    matrix.rotate(this->_rotation_y, QVector3D(0.0f, 1.0f, 0.0f));

    return matrix * QVector3D(0.0f, 0.0f, 1.0f);
}