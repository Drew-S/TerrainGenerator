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

#include "../Datatypes/vectormap.h"

// Setup the node
OutputNode::OutputNode()
{
    qDebug("Creating Output Node");
    this->_height_label = new QLabel("Height map");
    this->_height_label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    this->_height_label->setFixedSize(100, 100);

    this->_normal_label = new QLabel("Normal map");
    this->_normal_label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    this->_normal_label->setFixedSize(100, 100);

    QVBoxLayout *layout = new QVBoxLayout();

    layout->addWidget(this->_height_label);
    layout->addWidget(this->_normal_label);
    layout->setSizeConstraint(QLayout::SetMaximumSize);

    this->_widget = new QWidget();
    this->_widget->setLayout(layout);
    this->_widget->setStyleSheet("QWidget { background-color: rgba(0,0,0,0); } QLabel { background-color: rgba(117, 117, 117, 255); border: 1px solid black; }");

    QImage normal(1, 1, QImage::Format_Indexed8);
    normal.setColorCount(1);
    normal.setColor(0, qRgba(128, 128, 255, 255));
    normal.fill(0);

    this->_normal_map = normal;
}

OutputNode::~OutputNode() {}

// Title shown at the top of the node
QString OutputNode::caption() const
{
    return QString("Output");
}

// Title shown in the selection list
QString OutputNode::name() const
{
    return QString("Output");
}

// The image label that is embedded in the node
QWidget *OutputNode::embeddedWidget()
{
    Q_CHECK_PTR(this->_widget);
    return this->_widget;
}

// Get the number of in and out ports
unsigned int OutputNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 1 : 0;
}

// Get the output data (nothing)
std::shared_ptr<QtNodes::NodeData> OutputNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return nullptr;
}

// Get the data type for the port inputs and output
QtNodes::NodeDataType OutputNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    return IntensityMapData().type();
}

// Set the input pixmap data
void OutputNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    if (node_data)
    {
        // Cast pointer into VectorMapData pointer
        this->_input = std::dynamic_pointer_cast<IntensityMapData>(node_data);

        // Get width and height
        int w = this->_height_label->width();
        int h = this->_height_label->height();

        IntensityMap height_map = this->_input->intensityMap();

        this->_height_map = height_map.toImage();

        // Display preview image
        this->_height_label->setPixmap(height_map.toPixmap().scaled(w, h, Qt::KeepAspectRatio));

        // Emit that calculations are being made
        emit this->computingStarted();
        this->_generateNormalMap(height_map);
        emit this->computingFinished();

        QPixmap normal_pixmap;
        normal_pixmap.convertFromImage(this->getNormalMap(), Qt::ColorOnly);

        this->_normal_label->setPixmap(normal_pixmap.scaled(w, h, Qt::KeepAspectRatio));
    }
    // No pixmap, set null image
    else
    {

        this->_height_label->setText("Height map");
        this->_normal_label->setText("Normal map");
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
    return this->_height_map;
}

// Generates a normal map using the normal map generator
void OutputNode::_generateNormalMap(IntensityMap height_map)
{
    // Set the input height map for the generator
    this->_normal_generator.setImage(&height_map);

    // Generate
    // TODO: make multithreaded
    this->_normal_generator.generate();

    // Apply the normal map
    QImage normal_map = this->_normal_generator.toImage();
    this->_normal_map = normal_map;
}

// Input is removed so we reset the height and normal maps
void OutputNode::inputConnectionDeleted(QtNodes::Connection const &connection)
{
    Q_UNUSED(connection);
    this->_input = nullptr;
    this->_height_map = QImage();

    QImage normal(1, 1, QImage::Format_Indexed8);
    normal.setColorCount(1);
    normal.setColor(0, qRgba(128, 128, 255, 255));
    normal.fill(0);

    this->_normal_map = normal;
    emit this->computingFinished();
}

// Save the node for a file
QJsonObject OutputNode::save() const
{
    qDebug("Saving Output Node");
    QJsonObject data;
    data["name"] = this->name();
    return data;
}

// Restore the node from a file
void OutputNode::restore(QJsonObject const &data)
{
    Q_UNUSED(data);
    qDebug("Restore Output Node");
}
