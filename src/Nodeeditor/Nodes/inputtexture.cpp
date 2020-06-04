#include "inputtexture.h"

#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMouseEvent>
#include <QRegion>
#include <QToolButton>

#include <glm/vec4.hpp>

#include "../Datatypes/pixmap.h"
#include "Globals/drawing.h"
#include "Globals/settings.h"

/**
 * InputTextureNode
 * 
 * Creates the node and creates the UI.
 */
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
}

/**
 * created
 * 
 * Function is called when the node is created so it can connect to listeners.
 * 
 * @signals dataUpdated
 */
void InputTextureNode::created()
{
    QObject::connect(this->_ui.load_texture,
                     &QPushButton::clicked,
                     this,
                     &InputTextureNode::_loadFile);

    QObject::connect(this->_ui.new_texture,
                     &QPushButton::clicked,
                     this->_dialogue,
                     &QDialog::show);

    QObject::connect(this->_ui.edit_button, &QToolButton::clicked, [this]() {
        if (this->_texture_index != -1)
            DRAWING->show(this->_texture_index);
    });

    QObject::connect(this->_shared_ui.load_texture,
                     &QPushButton::clicked,
                     this,
                     &InputTextureNode::_loadFile);

    QObject::connect(this->_shared_ui.new_texture,
                     &QPushButton::clicked,
                     this->_dialogue,
                     &QDialog::show);

    QObject::connect(this->_shared_ui.edit_button,
                     &QToolButton::clicked,
                     [this]()
    {
        if (this->_texture_index != -1)
            DRAWING->show(this->_texture_index);
    });

    QObject::connect(this->_new_texture_ui.resolution,
                     QOverload<int>::of(&QSpinBox::valueChanged),
                     [this](int value)
    {
        this->_new_file_res = value;
    });
    QObject::connect(this->_new_texture_ui.ok_button,
                     &QPushButton::clicked,
                     this,
                     &InputTextureNode::_newFileAccept);

    QObject::connect(this->_new_texture_ui.cancel_button,
                     &QPushButton::clicked,
                     this->_dialogue,
                     &QDialog::reject);

    Q_CHECK_PTR(SETTINGS);
    QObject::connect(SETTINGS, &Settings::previewResolutionChanged, [this]() {
        emit this->dataUpdated(0);
    });
    QObject::connect(SETTINGS, &Settings::renderResolutionChanged, [this]() {
        qDebug("Render Resolution Changed");
        emit this->dataUpdated(0);
    });
    QObject::connect(SETTINGS, &Settings::renderModeChanged, [this]() {
        emit this->dataUpdated(0);
    });
}

/**
 * caption
 * 
 * Return a string that is displayed on the node and in the properties.
 * 
 * @returns QString : The caption.
 */
QString InputTextureNode::caption() const
{
    return "Texture";
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString InputTextureNode::name() const
{
    return "Texture";
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram.
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *InputTextureNode::embeddedWidget()
{
    Q_CHECK_PTR(this->_widget);
    return this->_widget;
}

/**
 * sharedWidget
 * 
 * Returns a pointer to the widget that gets displayed in the properties panel.
 * 
 * @returns QWidget* : The shared widget.
 */
QWidget *InputTextureNode::sharedWidget()
{
    Q_CHECK_PTR(this->_shared);
    return this->_shared;
}

/**
 * nPorts
 * 
 * Returns the number of ports the node has per type of port.
 * 
 * @param QtNodes::PortType port_type : The type of port to get the number of
 *                                      ports. QtNodes::PortType::In (input),
 *                                      QtNodes::PortType::Out (output)
 * 
 * @returns unsigned int : The number of ports.
 */
unsigned int InputTextureNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::Out ? 1 : 0;
}

/**
 * dataType
 * 
 * Returns the data type for each of the ports.
 * 
 * @param QtNodes::PortType port_type : The type of port (in or out).
 * @param QtNodes::PortIndex port_index : The port index on each side.
 * 
 * @returns QtNodes::NodeDataType : The type of data the port provides/accepts.
 */
QtNodes::NodeDataType
InputTextureNode::dataType(QtNodes::PortType port_type,
                           QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    return VectorMapData().type();
}

/**
 * setInData
 * 
 * Sets the input data on a port.
 * 
 * @param std::shared_ptr<QtNodes::NodeData> node_data : The shared pointer data
 *                                                       being inputted.
 * @param QtNodes::PortIndex port : The port the data is being set on.
 */
void InputTextureNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data,
                                 QtNodes::PortIndex port)
{
    Q_UNUSED(node_data);
    Q_UNUSED(port);
}

/**
 * outData
 * 
 * Returns a shared pointer for transport along a connection to another node.
 * 
 * @param QtNodes::PortIndex port : The port to get data from.
 * 
 * @returns std::shared_ptr<QtNodes::NodeData> : The shared output data.
 */
std::shared_ptr<QtNodes::NodeData>
InputTextureNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    Q_CHECK_PTR(SETTINGS);
    int size;
    if (SETTINGS->renderMode())
        size = SETTINGS->renderResolution();
    else
        size = SETTINGS->previewResolution();
    if (this->_texture != nullptr)
        return std::make_shared<VectorMapData>(this->_texture->vectorMap(size));

    return std::make_shared<VectorMapData>(VectorMap(size,
                                                     size,
                                                     glm::dvec4(0.00,
                                                                0.00,
                                                                0.00,
                                                                1.00)));
}

/**
 * _loadFile @slot
 * 
 * Called when the load button is pressed. The user selects an input texture
 * to load into the project.
 * 
 * @signals dataUpdated
 */
void InputTextureNode::_loadFile()
{
    // QFileDialog::getOpenFileName cannot be interacted with in a headless
    // testing environment Since we can assume QFileDialog works as expected we
    // use a fixed test file for testing
#ifdef TEST_MODE
    QString filename = QDir::cleanPath(QString(PWD)
                     + QString("/assets/textures/test.png"));
#else
    QString filename = QFileDialog::getOpenFileName(
        nullptr,
        tr("Open Image"),
        "/home/drew/Documents/School/Active/COMP_495/project/assets/textures",
        // QDir::homePath(),
        tr("Image Files (*.png *.jpg)"));
#endif

    Q_CHECK_PTR(TEXTURES);
    int index = TEXTURES->addTexture(filename);
    if (index != -1)
    {
        this->_texture_index = index;
        this->_texture = TEXTURES->at(index);
        QObject::connect(this->_texture,
                         &Texture::updated,
                         this,
                         &InputTextureNode::textureUpdated);

        this->_setPixmaps();
        emit this->dataUpdated(0);
    }
}

/**
 * _newFileAccept @slot
 * 
 * Called when the new texture dialogue has been accepted, this will create a
 * new blank texture.
 * 
 * @signals dataUpdated
 */
void InputTextureNode::_newFileAccept()
{
    this->_dialogue->accept();

    Q_CHECK_PTR(TEXTURES);
    int index = TEXTURES->addTexture(this->_new_file_res, this->_new_file_res);
    if (index != -1)
    {
        this->_texture_index = index;
        this->_new_file = true;
        this->_texture = TEXTURES->at(index);
        QObject::connect(this->_texture,
                         &Texture::updated,
                         this,
                         &InputTextureNode::textureUpdated);

        this->_setPixmaps();
        emit this->dataUpdated(0);
    }
}

/**
 * _setPixmaps @slot
 * 
 * Updates the labels that displays the loaded/created textures.
 */
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

/**
 * save
 * 
 * Saves the state of the node into a QJsonObject for the system to save to
 * file.
 * 
 * @returns QJsonObject : The saved state of the node.
 */
QJsonObject InputTextureNode::save() const
{
    Q_CHECK_PTR(SETTINGS);
    qDebug("Saving Input Texture Node");
    QJsonObject data;
    data["name"] = this->name();
    if (this->_texture)
    {
        if (SETTINGS->packImages() || this->_texture->generated())
        {
            data["image"] = this->_texture->saveName();
            data["stored"] = true;
        }
        else
        {
            data["image"] = this->_texture->filename();
            data["stored"] = false;
        }
    }
    else
    {
        data["image"] = "";
        data["stored"] = false;
    }

    return data;
}

/**
 * restore
 * 
 * Restores the state of the node from a provided json object.
 * 
 * @param QJsonObject const& data : The data to restore from.
 * 
 * @signals dataUpdated
 */
void InputTextureNode::restore(QJsonObject const &data)
{
    Q_CHECK_PTR(TEXTURES);
    qDebug("Restoring Input Texture Node");
    if (data["image"].toString() != QString(""))
    {
        int index;
        if (data["stored"].toBool())
            index = TEXTURES->find(data["image"].toString());
        else
            index = TEXTURES->addTexture(data["image"].toString());

        if (index != -1)
        {
            this->_texture_index = index;
            this->_texture = TEXTURES->at(index);
            QObject::connect(this->_texture,
                             &Texture::updated,
                             this,
                             &InputTextureNode::textureUpdated);
                             
            this->_setPixmaps();
            emit this->dataUpdated(0);
        }
    }
}

/**
 * textureUpdated @slot
 * 
 * Called when the attached texture is updated.
 * 
 * @signals dataUpdated
 */
void InputTextureNode::textureUpdated()
{
    this->_setPixmaps();
    emit this->dataUpdated(0);
}