#include "output.h"

#include <QColor>
#include <QDebug>
#include <QList>
#include <QMatrix>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QVector3D>
#include <QWidget>

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

#include "../Datatypes/vectormap.h"
#include "Globals/settings.h"

/**
 * OutputNode
 * 
 * Create the output node, the UI, and default normal map.
 */
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

    QImage albedo(1, 1, QImage::Format_Indexed8);
    albedo.setColorCount(1);
    albedo.setColor(0, qRgba(210, 210, 210, 255));
    albedo.fill(0);
    this->_albedo_map = albedo;

    QObject::connect(&this->_normal_generator,
                     &NormalMapGenerator::done,
                     [this]()
    {
        this->_normal_map = this->_normal_generator.toImage();
        emit this->computingFinished();

        QPixmap normal_pixmap;
        normal_pixmap.convertFromImage(this->getNormalMap(), Qt::ColorOnly);

        this->_ui.normal_label->setPixmap(
            normal_pixmap.scaled(
                this->_ui.height_label->width(),
                this->_ui.height_label->height(),
                Qt::KeepAspectRatio));

        this->_ui.progress->hide();
    });

    QObject::connect(&this->_normal_generator,
                     &NormalMapGenerator::started,
                     [this]()
    {
        Q_CHECK_PTR(SETTINGS);
        
        this->_ui.progress->show();

        if (SETTINGS->percentProgressText()
            && !this->_ui.progress->isTextVisible())
            this->_ui.progress->setTextVisible(true);
        else if (!SETTINGS->percentProgressText()
                 && this->_ui.progress->isTextVisible())
            this->_ui.progress->setTextVisible(false);

        this->_ui.progress->setValue(0);
    });
    QObject::connect(&this->_normal_generator,
                     &NormalMapGenerator::progress,
                     this->_ui.progress,
                     &QProgressBar::setValue);
}

/**
 * caption
 * 
 * Return a string that is displayed on the node and in the properties.
 * 
 * @returns QString : The caption.
 */
QString OutputNode::caption() const
{
    return QString("Output");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString OutputNode::name() const
{
    return QString("Output");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram.
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *OutputNode::embeddedWidget()
{
    Q_CHECK_PTR(this->_widget);
    return this->_widget;
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
unsigned int OutputNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 2 : 0;
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
std::shared_ptr<QtNodes::NodeData> OutputNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return nullptr;
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
OutputNode::dataType(QtNodes::PortType port_type,
                     QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_type);
    switch ((int)port_index)
    {
    case 0:
        return IntensityMapData().type();
        break;
    case 1:
        return VectorMapData().type();
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
    Q_UNREACHABLE();
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
void OutputNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data,
                           QtNodes::PortIndex port)
{
    if (node_data)
    {
        switch((int)port)
        {
        case 0:
            // Cast pointer into IntensityMapData pointer
            if((this->_input =
                std::dynamic_pointer_cast<IntensityMapData>(node_data)))
            {
                IntensityMap height_map = this->_input->intensityMap();

                this->_height_map = height_map.toImage();

                // Display preview image
                this->_ui.height_label->setPixmap(
                    height_map.toPixmap().scaled(
                        this->_ui.height_label->width(),
                        this->_ui.height_label->height(),
                        Qt::KeepAspectRatio));

                // Emit that calculations are being made
                this->_generateNormalMap(height_map);
            }
            break;
        case 1:
            // Cast pointer into VectorMapData pointer
            if((this->_input_albedo =
                std::dynamic_pointer_cast<VectorMapData>(node_data)))
            {
                this->_albedo_map = this->_input_albedo->vectorMap().toImage();
                emit this->computingFinished();
            }
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
    }
    // No pixmap, set null image
    else
    {
        this->_ui.height_label->setText("Height map");
        this->_ui.normal_label->setText("Normal map");
    }
}

/**
 * getNormalMap
 * 
 * Returns the generated normal map.
 * 
 * @returns QImage : The normal map.
 */
QImage OutputNode::getNormalMap()
{
    return this->_normal_map;
}

/**
 * getHeightMap
 * 
 * Returns the height map.
 * 
 * @returns QImage : The height map.
 */
QImage OutputNode::getHeightMap()
{
    return this->_height_map;
}

/**
 * getAlbedoMap
 * 
 * Returns the albedo map.
 * 
 * @returns QImage : The albedo map.
 */
QImage OutputNode::getAlbedoMap()
{
    return this->_albedo_map;
}

/**
 * _generateNormalMap
 * 
 * Given the height map (intensity map) this will generate the normal map.
 * 
 * @param IntensityMap height_map : The height map.
 * 
 * @signals computingStarted
 */
void OutputNode::_generateNormalMap(IntensityMap height_map)
{
    emit this->computingStarted();
    // Set the input height map for the generator
    this->_normal_generator.setImage(height_map);

    // Generate
    this->_normal_generator.generate();
}

/**
 * inputConnectionDeleted @slot
 * 
 * Called when an input connection is deleted, this usually resets some data and
 * regenerates the output data.
 * 
 * @param QtNodes::Connection const& connection : The connection being deleted.
 * 
 * @signals computingFinished
 */
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

/**
 * save
 * 
 * Saves the state of the node into a QJsonObject for the system to save to
 * file.
 * 
 * @returns QJsonObject : The saved state of the node.
 */
QJsonObject OutputNode::save() const
{
    qDebug("Saving Output Node");
    QJsonObject data;
    data["name"] = this->name();
    return data;
}

/**
 * restore
 * 
 * Restores the state of the node from a provided json object.
 * 
 * @param QJsonObject const& data : The data to restore from.
 */
void OutputNode::restore(QJsonObject const &data)
{
    Q_UNUSED(data);
    qDebug("Restore Output Node");
}
