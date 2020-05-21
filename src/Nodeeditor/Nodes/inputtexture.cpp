#include "inputtexture.h"

#include <QDebug>
#include <QFileDialog>
#include <QRegion>
#include <QFileInfo>
#include <QMouseEvent>
#include <QToolButton>

#include "../Datatypes/pixmap.h"

#include "Globals/drawing.h"

#include <glm/vec4.hpp>

// Setup the node
InputTextureNode::InputTextureNode()
{
    qDebug("Creating Input Texture Node, attaching UI");
    // Create an image container
    this->_widget = new QWidget();
    this->_shared = new QWidget();
    this->_dialogue = new QDialog();

    this->_new_texture_ui.setupUi(this->_dialogue);
    this->_shared_ui.setupUi(this->_shared);
    this->_ui.setupUi(this->_widget);

    QObject::connect(this->_ui.load_texture, &QPushButton::clicked, this, &InputTextureNode::_loadFile);
    QObject::connect(this->_ui.new_texture, &QPushButton::clicked, this->_dialogue, &QDialog::show);
    QObject::connect(this->_ui.edit_button, &QToolButton::clicked, [this]() {
        if (this->_texture_index != -1)
            DRAWING->show(this->_texture_index);
    });

    QObject::connect(this->_shared_ui.load_texture, &QPushButton::clicked, this, &InputTextureNode::_loadFile);
    QObject::connect(this->_shared_ui.new_texture, &QPushButton::clicked, this->_dialogue, &QDialog::show);
    QObject::connect(this->_shared_ui.edit_button, &QToolButton::clicked, [this]() {
        if (this->_texture_index != -1)
            DRAWING->show(this->_texture_index);
    });

    QObject::connect(this->_new_texture_ui.resolution, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        this->_new_file_res = value;
    });
    QObject::connect(this->_new_texture_ui.ok_button, &QPushButton::clicked, this, &InputTextureNode::_newFileAccept);
    QObject::connect(this->_new_texture_ui.cancel_button, &QPushButton::clicked, this->_dialogue, &QDialog::reject);
}

InputTextureNode::~InputTextureNode() {}

// Title shown at the top of the node
QString InputTextureNode::caption() const
{
    return "Texture";
}

// Title shown in the selection list
QString InputTextureNode::name() const
{
    return "Texture";
}

// The image label that is embedded in the node
QWidget *InputTextureNode::embeddedWidget()
{
    return this->_widget;
}

// The image label that is shared with the properties
QWidget *InputTextureNode::sharedWidget()
{
    return this->_shared;
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
    if (this->_texture != nullptr)
        return std::make_shared<VectorMapData>(this->_texture->vectorMap());

    return std::make_shared<VectorMapData>(VectorMap(1, 1, glm::dvec4(0.00, 0.00, 0.00, 1.00)));
}

void InputTextureNode::_loadFile()
{
    // QFileDialog::getOpenFileName cannot be interacted with in a headless testing environment
    // Since we can assume QFileDialog works as expected we use a fixed test file for testing
#ifdef TEST_MODE
    QString filename = QDir::cleanPath(QString(PWD) + QString("/assets/textures/test.png"));
#else
    QString filename = QFileDialog::getOpenFileName(
        nullptr,
        tr("Open Image"),
        "/home/drew/Documents/School/Active/COMP_495/project/assets/textures",
        // QDir::homePath(),
        tr("Image Files (*.png *.jpg)"));
#endif

    int index = TEXTURES->addTexture(filename);
    if (index != -1)
    {
        this->_texture_index = index;
        this->_texture = TEXTURES->at(index);
        QObject::connect(this->_texture, &Texture::updated, this, &InputTextureNode::textureUpdated);
        this->_setPixmaps();
        emit this->dataUpdated(0);
    }
}

void InputTextureNode::_newFileAccept()
{
    this->_dialogue->accept();

    int index = TEXTURES->addTexture(this->_new_file_res, this->_new_file_res);
    if (index != -1)
    {
        this->_texture_index = index;
        this->_new_file = true;
        this->_texture = TEXTURES->at(index);
        QObject::connect(this->_texture, &Texture::updated, this, &InputTextureNode::textureUpdated);
        this->_setPixmaps();
        emit this->dataUpdated(0);
    }
}

void InputTextureNode::_setPixmaps()
{
    if (this->_texture != nullptr)
    {
        this->_ui.texture_display->setPixmap(
            this->_texture->pixmap().scaled(
                this->_ui.texture_display->width(),
                this->_ui.texture_display->height(),
                Qt::KeepAspectRatio));
        this->_shared_ui.texture_display->setPixmap(
            this->_texture->pixmap().scaled(
                this->_shared_ui.texture_display->width(),
                this->_shared_ui.texture_display->height(),
                Qt::KeepAspectRatio));
    }
    else
    {
        this->_ui.texture_display->setText("No Image");
        this->_shared_ui.texture_display->setText("No Image");
    }
}

// Save the node to for a file
QJsonObject InputTextureNode::save() const
{
    qDebug("Saving Input Texture Node");
    QJsonObject data;
    data["name"] = this->name();
    data["image_generated"] = this->_new_file;
    data["image"] = this->_texture->filename();
    return data;
}

// Restore the node from a save
void InputTextureNode::restore(QJsonObject const &data)
{
    qDebug("Restoring Input Texture Node");
    // this->_filename = data["image"].toString();
}

void InputTextureNode::textureUpdated()
{
    this->_setPixmaps();
    emit this->dataUpdated(0);
}