#include "output.h"

#include "../Datatypes/pixmap.h"

// Setup the node
OutputNode::OutputNode()
{
    // Create an image container
    this->_widget = new QLabel("Output");
    this->_widget->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    this->_widget->setFixedSize(100, 100);
    this->_widget->setPixmap(QPixmap());

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
        int w = this->_widget->width();
        int h = this->_widget->height();

        // Display preview image
        this->_widget->setPixmap(pixmap->pixmap().scaled(w, h, Qt::KeepAspectRatio));

        // Emit that calculations are being made
        emit this->computingStarted();
        this->generateNormalMap();
        emit this->computingFinished();
    }
    // No pixmap, set null image
    else
    {
        this->_widget->setPixmap(QPixmap());
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

// TODO: Implement the normal map generation
void OutputNode::generateNormalMap(){};