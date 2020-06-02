#include "output.h"

#include <QList>
#include <QVector3D>
#include <QMatrix>
#include <QColor>
#include <QDebug>
#include <QWidget>
#include <QProgressBar>
#include <QVBoxLayout>

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

#include "../Datatypes/vectormap.h"

#include "Globals/settings.h"

// Setup the node
OutputNode::OutputNode()
{
    qDebug("Creating Output Node");
    this->_widget = new QWidget();
    this->_ui.setupUi(this->_widget);

    this->_ui.progress->hide();

    QImage normal(1, 1, QImage::Format_Indexed8);
    normal.setColorCount(1);
    normal.setColor(0, qRgba(128, 128, 255, 255));
    normal.fill(0);

    this->_normal_map = normal;

    QObject::connect(&this->_normal_generator, &NormalMapGenerator::done, [this]() {
        this->_normal_map = this->_normal_generator.toImage();
        emit this->computingFinished();

        QPixmap normal_pixmap;
        normal_pixmap.convertFromImage(this->getNormalMap(), Qt::ColorOnly);

        this->_ui.normal_label->setPixmap(normal_pixmap.scaled(this->_ui.height_label->width(), this->_ui.height_label->height(), Qt::KeepAspectRatio));
        this->_ui.progress->hide();
    });

    QObject::connect(&this->_normal_generator, &NormalMapGenerator::started, [this]() {
        Q_CHECK_PTR(SETTINGS);
        
        this->_ui.progress->show();

        if (SETTINGS->percentProgressText() && !this->_ui.progress->isTextVisible())
            this->_ui.progress->setTextVisible(true);
        else if (!SETTINGS->percentProgressText() && this->_ui.progress->isTextVisible())
            this->_ui.progress->setTextVisible(false);

        this->_ui.progress->setValue(0);
    });
    QObject::connect(&this->_normal_generator, &NormalMapGenerator::progress, this->_ui.progress, &QProgressBar::setValue);
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

        IntensityMap height_map = this->_input->intensityMap();

        this->_height_map = height_map.toImage();

        // Display preview image
        this->_ui.height_label->setPixmap(height_map.toPixmap().scaled(this->_ui.height_label->width(), this->_ui.height_label->height(), Qt::KeepAspectRatio));

        // Emit that calculations are being made
        this->_generateNormalMap(height_map);
    }
    // No pixmap, set null image
    else
    {

        this->_ui.height_label->setText("Height map");
        this->_ui.normal_label->setText("Normal map");
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
    emit this->computingStarted();
    // Set the input height map for the generator
    this->_normal_generator.setImage(height_map);

    // Generate
    this->_normal_generator.generate();
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
