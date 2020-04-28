#pragma once

#include <vector>

#include <QImage>
#include <QPixmap>
#include <QColor>

#include <glm/vec3.hpp>

class IntensityMap
{
public:
    enum Channel
    {
        RED,
        GREEN,
        BLUE,
        AVERAGE,
        MIN,
        MAX
    };
    IntensityMap(){};
    IntensityMap(int width, int height)
    {
        this->width = width;
        this->height = height;
    };
    IntensityMap(int width, int height, std::vector<double> values)
    {
        this->width = width;
        this->height = height;
        this->values = values;
    };
    IntensityMap(QImage image, IntensityMap::Channel channel)
    {
        this->_saveImage(image, channel);
    };
    IntensityMap(QPixmap image, IntensityMap::Channel channel)
    {
        this->_saveImage(image.toImage(), channel);
    };
    ~IntensityMap(){};

    QImage toImage()
    {
        QImage image(this->width, this->height, QImage::Format_RGBA64);
        for (int x = 0; x < this->width; x++)
        {
            for (int y = 0; y < this->height; y++)
            {
                double h = this->at(x, y);
                QColor color = QColor::fromRgbF(h, h, h);
                image.setPixelColor(x, y, color);
            }
        }
        return image;
    };

    QPixmap toPixmap() { return QPixmap::fromImage(this->toImage()); };

    double
    at(int x, int y)
    {
        return this->values[y * this->width + x];
    };

    void append(double value)
    {
        this->values.push_back(value);
    };

    void set(int x, int y, double v)
    {
        this->values[y * this->width + x] = v;
    };

    int width;
    int height;
    std::vector<double> values;

private:
    void _saveImage(QImage image, IntensityMap::Channel channel)
    {
        this->width = image.width();
        this->height = image.height();
        for (int x = 0; x < this->width; x++)
        {
            for (int y = 0; y < this->height; y++)
            {
                double c, min, max;
                QColor color = image.pixelColor(x, y);
                switch (channel)
                {
                case IntensityMap::RED:
                    this->values.push_back(color.redF());
                    break;

                case IntensityMap::GREEN:
                    this->values.push_back(color.greenF());
                    break;

                case IntensityMap::BLUE:
                    this->values.push_back(color.blueF());
                    break;

                case IntensityMap::AVERAGE:
                    this->values.push_back((color.redF() + color.greenF() + color.blueF()) / 3.00);
                    break;

                case IntensityMap::MIN:
                    min = color.redF();
                    c = color.greenF();
                    if (c < min)
                        min = c;
                    c = color.blueF();
                    if (c < min)
                        min = c;
                    this->values.push_back(min);
                    break;

                case IntensityMap::MAX:
                    max = color.redF();
                    c = color.greenF();
                    if (c > max)
                        max = c;
                    c = color.blueF();
                    if (c > max)
                        max = c;
                    this->values.push_back(max);
                    break;
                }
            }
        }
    };
};

class VectorMap
{
public:
    VectorMap(){};
    VectorMap(int width, int height)
    {
        this->width = width;
        this->height = height;
    };
    VectorMap(int width, int height, std::vector<glm::dvec3> values)
    {
        this->width = width;
        this->height = height;
        this->values = values;
    };
    VectorMap(QImage image)
    {
        this->_saveImage(image);
    };
    VectorMap(QPixmap image)
    {
        this->_saveImage(image.toImage());
    };
    ~VectorMap(){};

    QImage toImage()
    {
        QImage image(this->width, this->height, QImage::Format_RGBA64);
        for (int x = 0; x < this->width; x++)
        {
            for (int y = 0; y < this->height; y++)
            {
                glm::dvec3 pixel = this->at(x, y);
                QColor color = QColor::fromRgbF(pixel.x, pixel.y, pixel.z);
                image.setPixelColor(x, y, color);
            }
        }
        return image;
    };

    QPixmap toPixmap() { return QPixmap::fromImage(this->toImage()); };

    glm::dvec3 at(int x, int y)
    {
        return this->values[y * this->width + x];
    }

    void append(glm::dvec3 value)
    {
        this->values.push_back(value);
    };

    int width;
    int height;
    std::vector<glm::dvec3> values;

private:
    void _saveImage(QImage image)
    {
        this->width = image.width();
        this->height = image.height();
        for (int x = 0; x < this->width; x++)
        {
            for (int y = 0; y < this->height; y++)
            {
                QColor color = image.pixelColor(x, y);
                this->values.push_back(glm::dvec3(color.redF(), color.greenF(), color.blueF()));
            }
        }
    };
};