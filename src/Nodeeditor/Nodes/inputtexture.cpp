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

InputTextureNode::~InputTextureNode() {}

// Title shown at the top of the node
QString InputTextureNode::caption() const
{
    return "Input Texture";
}

// Title shown in the selection list
QString InputTextureNode::name() const
{
    return "InputTextureNode";
}
void InputTextureNode::name(QString name)
{
    (void)name;
}

// The image label that is embedded in the node
QWidget *InputTextureNode::embeddedWidget()
{
    return this->_widget;
}

// Needed for NodeDataModel, not sure where it is used
QString InputTextureNode::modelName()
{
    return "Input Texture Node";
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
    return VectorMapData().type();
}

// Needed for all nodes, even if there are no inputs
void InputTextureNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
{
    (void)node_data;
    (void)port;
}

// Get the data attached to a port
std::shared_ptr<QtNodes::NodeData> InputTextureNode::outData(QtNodes::PortIndex port)
{
    (void)port;
    return std::make_shared<VectorMapData>(this->_color_map);
}

// When clicking on the node, if user clicks the QLabel image container
// open a file select dialog and select a file.
// If the node is resized, updated the image size
bool InputTextureNode::eventFilter(QObject *object, QEvent *event)
{
    if (object == this->_widget)
    {
        // Click to select an image
        if (event->type() == QEvent::MouseButtonPress)
        {
            // Select a texture file

            this->_filename = QFileDialog::getOpenFileName(
                nullptr,
                tr("Open Image"),
                "/home/drew/Documents/School/Active/COMP_495/project/assets/textures",
                // QDir::homePath(),
                tr("Image Files (*.png *.jpg)"));

            this->_generate();
            return true;
        }
        // Resize the selecting pixmap
        else if (event->type() == QEvent::Resize)
        {
            if (!this->_pixmap.isNull())
            {
                // Scale the pixmap to fill the window
                this->_widget->setPixmap(this->_pixmap.scaled(this->_widget->width(), this->_widget->height(), Qt::KeepAspectRatio));
            }
        }
    }
    return false;
}

void InputTextureNode::_generate()
{
    // If a file is not selected set the QLabel to display
    // a select image message
    if (this->_filename == "")
    {
        this->_widget->setText("Click to load image");
    }

    // If a file is selected set the image preview
    else
    {
        this->_pixmap = QPixmap(this->_filename);
        this->_color_map = VectorMap(this->_pixmap);
        this->_widget->setPixmap(this->_pixmap.scaled(this->_widget->width(), this->_widget->height(), Qt::KeepAspectRatio));
    }

    // Inform that data is updated so pipline is updated
    emit this->dataUpdated(0);
}

// Save the node to for a file
QJsonObject InputTextureNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    data["image"] = this->_filename;
    return data;
}

// Restore the node from a save file
void InputTextureNode::restore(QJsonObject const &data)
{
    this->_filename = data["image"].toString();
    this->_generate();
}