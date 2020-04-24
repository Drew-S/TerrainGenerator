#include "output.h"

#include <QList>
#include <QVector3D>
#include <QMatrix>
#include <QColor>
#include <QDebug>
#include <QWidget>
#include <QVBoxLayout>

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

#include "../Datatypes/pixmap.h"

// Setup the node
OutputNode::OutputNode()
{
    this->_height_label = new QLabel("Height map");
    this->_height_label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    this->_height_label->setFixedSize(80, 80);
    this->_height_label->setPixmap(QPixmap());

    this->_normal_label = new QLabel("Normal map");
    this->_normal_label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    this->_normal_label->setFixedSize(80, 80);
    this->_normal_label->setPixmap(QPixmap());

    QVBoxLayout *layout = new QVBoxLayout();

    layout->addWidget(this->_height_label);
    layout->addWidget(this->_normal_label);
    layout->setSizeConstraint(QLayout::SetMaximumSize);

    this->_widget = new QWidget();
    this->_widget->setLayout(layout);

    QImage normal(1, 1, QImage::Format_Indexed8);
    normal.setColorCount(1);
    normal.setColor(0, qRgba(128, 128, 255, 255));
    normal.fill(0);

    this->_normal_map = normal;
}

// Get the number of in and out ports
unsigned int OutputNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 1 : 0;
}

// Get the data type for the port inputs and output
QtNodes::NodeDataType OutputNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    (void)port_type;
    (void)port_index;
    return PixmapData().type();
}

// Set the input pixmap data
void OutputNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
{
    (void)port;
    // Save pointer
    this->_pixmap = node_data;
    if (this->_pixmap)
    {
        // Cast pointer into PixmapData pointer
        std::shared_ptr<PixmapData> pixmap = std::dynamic_pointer_cast<PixmapData>(this->_pixmap);

        // Get width and height
        int w = this->_height_label->width();
        int h = this->_height_label->height();

        // Display preview image
        this->_height_label->setPixmap(pixmap->pixmap().scaled(w, h, Qt::KeepAspectRatio));

        // Emit that calculations are being made
        emit this->computingStarted();
        this->generateNormalMap();
        emit this->computingFinished();

        QPixmap normal_pixmap;
        normal_pixmap.convertFromImage(this->getNormalMap(), Qt::ColorOnly);

        this->_normal_label->setPixmap(normal_pixmap.scaled(w, h, Qt::KeepAspectRatio));
    }
    // No pixmap, set null image
    else
    {
        this->_height_label->setPixmap(QPixmap());
        this->_normal_label->setPixmap(QPixmap());
    }
}

// Returns the generated normal map
QImage OutputNode::getNormalMap()
{
    return this->_normal_map;
}

// Returns the generated heightmap
QImage OutputNode::getHeightMap()
{
    std::shared_ptr<PixmapData> pixmap = std::dynamic_pointer_cast<PixmapData>(this->_pixmap);
    return pixmap->pixmap().toImage();
}

// Generates a normal map using the normal map generator
void OutputNode::generateNormalMap()
{
    QImage height_map = this->getHeightMap();

    // Set the input height map for the generator
    this->_normal_generator.setImage(&height_map);

    // Generate
    // TODO: make multithreaded
    this->_normal_generator.generate();

    // Apply the normal map
    QImage normal_map = this->_normal_generator.toImage();
    this->_normal_map = normal_map;

    // TODO: remove, used for testing
    // normal_map.save("/home/drew/test_normal_map.png");
};