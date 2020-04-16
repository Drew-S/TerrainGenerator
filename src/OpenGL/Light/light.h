#pragma once

#include <QColor>
#include <QVector3D>

class Light
{
public:
    // Light is considered a sun, so no position only direction
    Light(){};
    Light(float rotation_x, float rotation_y, float intensity, QColor color);
    ~Light(){};

    QVector3D direction();

    float intensity() { return this->_intensity; };

    QColor color() { return this->_color; };

    void rotateY(float rotation) { this->_rotation_y += rotation; };
    void rotateX(float rotation) { this->_rotation_x += rotation; };
    void setRotationY(float rotation) { this->_rotation_y = rotation; };
    void setRotationX(float rotation) { this->_rotation_x = rotation; };
    void setIntensity(float intensity) { this->_intensity = intensity; };
    void setColor(QColor color) { this->_color = color; };

private:
    float _rotation_x = 0.0f;
    float _rotation_y = 0.0f;
    float _intensity = 1.0f;
    QColor _color{255, 255, 255, 255};
};