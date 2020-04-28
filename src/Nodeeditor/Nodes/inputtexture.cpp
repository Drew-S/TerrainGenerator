#include "inputtexture.h"

#include <QFileDialog>

#include "../Datatypes/pixmap.h"

// Setup the node
InputTextureNode::InputTextureNode()
{
    // Create an image container
    this->_widget = new QLabel("Click to load image");
    this->_widget->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    this->_widget->setFixedSize(100, 100);
    this->_widget->installEventFilter(this);
}

// Get the number of in and out ports
unsigned int InputTextureNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::Out ? 1 : 0;
}

// Get the data type for the port inputs and output
QtNodes::NodeDataType InputTextureNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    (void)port_type;
    (void)port_index;
    return PixmapData().type();
}

// Get the data attached to a port
std::shared_ptr<QtNodes::NodeData> InputTextureNode::outData(QtNodes::PortIndex port)
{
    (void)port;
    return std::make_shared<PixmapData>(this->_color_map);
}

// When clicking on the node, if user clicks the QLabel image container
// open a file select dialog and select a file.
// If the node is resized, updated the image size
bool InputTextureNode::eventFilter(QObject *object, QEvent *event)
{
    if (object == this->_widget)
    {
        // Get label size
        int w = this->_widget->width();
        int h = this->_widget->height();

        // Click to select an image
        if (event->type() == QEvent::MouseButtonPress)
        {
            // Select a texture file
            QString file_name = QFileDialog::getOpenFileName(
                nullptr,
                tr("Open Image"),
                "/home/drew/Documents/School/Active/COMP_495/project/assets/textures",
                // QDir::homePath(),
                tr("Image Files (*.png *.jpg)"));

            // If a file is not selected set the QLabel to display
            // a select image message
            if (file_name == "")
            {
                this->_widget->setText("Click to load image");
            }

            // If a file is selected set the image preview
            else
            {
                this->_pixmap = QPixmap(file_name);
                this->_color_map = VectorMap(this->_pixmap);
                this->_widget->setPixmap(this->_pixmap.scaled(w, h, Qt::KeepAspectRatio));
            }

            // Inform that data is updated so pipline is updated
            emit this->dataUpdated(0);
            return true;
        }
        // Resize the selecting pixmap
        else if (event->type() == QEvent::Resize)
        {
            if (!this->_pixmap.isNull())
            {
                // Scale the pixmap to fill the window
                this->_widget->setPixmap(this->_pixmap.scaled(w, h, Qt::KeepAspectRatio));
            }
        }
    }
    return false;
}