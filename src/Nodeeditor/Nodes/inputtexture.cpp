#include "inputtexture.h"

#include <QDebug>
#include <QFileDialog>
#include <QRegion>
#include <QFileInfo>
#include <QMouseEvent>

#include "../Datatypes/pixmap.h"

// Setup the node
InputTextureNode::InputTextureNode()
{
    this->_global = GlobalData::getInstance();
    qDebug("Creating Input Texture Node, attaching UI");
    // Create an image container
    this->_widget = new QWidget();
    this->_shared = new QWidget();
    this->_dialogue = new QDialog();

    this->_new_texture_ui.setupUi(this->_dialogue);
    this->_drawing_ui.setupUi(this->_shared);
    this->_ui.setupUi(this->_widget);

    QObject::connect(this->_ui.load_texture, &QPushButton::clicked, this, &InputTextureNode::_loadFile);
    QObject::connect(this->_ui.new_texture, &QPushButton::clicked, this->_dialogue, &QDialog::exec);
    QObject::connect(this->_new_texture_ui.resolution, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        this->_new_file_res = value;
    });
    QObject::connect(this->_new_texture_ui.ok_button, &QPushButton::clicked, this, &InputTextureNode::_newFileAccept);
    QObject::connect(this->_new_texture_ui.cancel_button, &QPushButton::clicked, this->_dialogue, &QDialog::reject);

    QObject::connect(this->_drawing_ui.brush_size, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_brush = value;
        this->_stencils->stencil(0)->setBrush(this->_brush);
    });
    QObject::connect(this->_drawing_ui.flow_rate, &QSlider::valueChanged, [this](int value) {
        this->_flow = (double)value;
        this->_drawing_ui.flow_rate_label->setText(QString::number(value));
    });
    QObject::connect(this->_drawing_ui.color_strength, &QSlider::valueChanged, [this](int value) {
        this->_color.setRed(value);
        this->_color.setGreen(value);
        this->_color.setBlue(value);
        this->_stencils->stencil(0)->setColor(this->_color);
    });
    QObject::connect(this->_drawing_ui.opacity, &QSlider::valueChanged, [this](int value) {
        this->_color.setAlpha(value);
        this->_stencils->stencil(0)->setColor(this->_color);
    });

    this->_drawing_ui.drawing_surface->installEventFilter(this);

    this->_stencils = StencilList::getInstance();
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

// The image label that is shared with the properties
QWidget *InputTextureNode::sharedWidget()
{
    return this->_shared;
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

void InputTextureNode::_loadFile()
{
    this->_filename = QFileDialog::getOpenFileName(
        nullptr,
        tr("Open Image"),
        "/home/drew/Documents/School/Active/COMP_495/project/assets/textures",
        // QDir::homePath(),
        tr("Image Files (*.png *.jpg)"));

    this->_generate();
}

void InputTextureNode::_newFileAccept()
{
    this->_dialogue->accept();
    this->_pixmap = new QPixmap(this->_new_file_res, this->_new_file_res);
    this->_painter = new QPainter(this->_pixmap);
    this->_painter->setBrush(QBrush(QColor(255, 255, 255, 255)));
    this->_painter->drawRect(QRect(0, 0, this->_new_file_res, this->_new_file_res));

    this->_color_map = VectorMap(this->_new_file_res, this->_new_file_res);
    this->_new_file = true;
    this->_filename = "new_texture.png";
    this->_setPixmaps();

    emit this->dataUpdated(0);
}

void InputTextureNode::_setPixmaps()
{
    this->_drawing_ui.drawing_surface->setPixmap(
        this->_pixmap->scaled(
            this->_drawing_ui.drawing_surface->width(),
            this->_drawing_ui.drawing_surface->height(),
            Qt::KeepAspectRatio));
    this->_ui.texture_display->setPixmap(
        this->_pixmap->scaled(
            this->_ui.texture_display->width(),
            this->_ui.texture_display->height(),
            Qt::KeepAspectRatio));
}

void InputTextureNode::_generate()
{
    // If a file is not selected set the QLabel to display
    // a select image message
    if (this->_filename == "")
    {
        this->_ui.texture_display->setText("No Image");
        this->_drawing_ui.drawing_surface->setText("No Image");
    }

    // If a file is selected set the image preview
    else
    {
        qDebug("Setting input texture");
        this->_pixmap = new QPixmap(this->_filename);
        this->_painter = new QPainter(this->_pixmap);
        this->_painter->setBrush(QBrush(QColor(255, 255, 255, 255)));
        this->_painter->drawRect(QRect(0, 0, this->_new_file_res, this->_new_file_res));

        this->_color_map = VectorMap(this->_pixmap->scaled(this->_pixmap->width(), this->_pixmap->height(), Qt::KeepAspectRatio));
        this->_ui.filepath->setText(QFileInfo(this->_filename).fileName());
        this->_ui.filepath->setToolTip(this->_filename);
        this->_setPixmaps();
    }

    // Inform that data is updated so pipline is updated
    emit this->dataUpdated(0);
}

// Save the node to for a file
QJsonObject InputTextureNode::save() const
{
    qDebug("Saving Input Texture Node");
    QJsonObject data;
    data["name"] = this->name();
    data["image"] = this->_filename;
    data["image_generated"] = this->_new_file;
    if (this->_new_file)
        this->_pixmap->save(this->_global->tmpDir() + "/" + this->_filename);
    return data;
}

// Restore the node from a save
void InputTextureNode::restore(QJsonObject const &data)
{
    qDebug("Restoring Input Texture Node");
    this->_filename = data["image"].toString();
    this->_generate();
}

bool InputTextureNode::eventFilter(QObject *object, QEvent *event)
{
    if (object == this->_drawing_ui.drawing_surface && this->_filename != "")
    {
        if (event->type() == QEvent::MouseButtonRelease)
        {
            this->_prev_first = false;
            return true;
        }
        if (!(event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseMove))
            return false;

        QMouseEvent *mouse_event = dynamic_cast<QMouseEvent *>(event);

        if (mouse_event->buttons() != Qt::LeftButton)
            return false;

        QPointF pos = mouse_event->localPos();
        pos.setX(pos.x() * this->_pixmap->width() / 128.00);
        pos.setY(pos.y() * this->_pixmap->width() / 128.00);

        if (!this->_prev_first)
        {
            this->_prev_first = true;
            this->_prev = pos;
            this->_stencils->stencil(0)->draw(this->_painter, pos);

            this->_drawing_ui.drawing_surface->setPixmap(this->_pixmap->scaled(128, 128, Qt::KeepAspectRatio));

            this->_ui.texture_display->setPixmap(
                this->_pixmap->scaled(
                    this->_ui.texture_display->width(),
                    this->_ui.texture_display->height(),
                    Qt::KeepAspectRatio));
            this->_color_map = VectorMap(this->_pixmap->scaled(this->_pixmap->width(), this->_pixmap->height(), Qt::KeepAspectRatio));

            emit this->dataUpdated(0);
            return true;
        }
        else if (QLineF(this->_prev, pos).length() >= 101.00 - this->_flow)
        {
            this->_stencils->stencil(0)->draw(this->_painter, pos);

            this->_drawing_ui.drawing_surface->setPixmap(this->_pixmap->scaled(128, 128, Qt::KeepAspectRatio));

            this->_ui.texture_display->setPixmap(
                this->_pixmap->scaled(
                    this->_ui.texture_display->width(),
                    this->_ui.texture_display->height(),
                    Qt::KeepAspectRatio));
            this->_color_map = VectorMap(this->_pixmap->scaled(this->_pixmap->width(), this->_pixmap->height(), Qt::KeepAspectRatio));

            this->_prev = pos;

            emit this->dataUpdated(0);
            return true;
        }
    }
    return false;
}