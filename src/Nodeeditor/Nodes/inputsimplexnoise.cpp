#include "inputsimplexnoise.h"

#include <math.h>

#include <QColor>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QImage>
#include <QProgressBar>

#include <glm/vec4.hpp>

#include "../Datatypes/pixmap.h"
#include "Globals/settings.h"

/******************************************************************************
 *                                 WORKER                                     *
 ******************************************************************************/

/**
 * set
 * 
 * Sets the parameters used by the thread. This should only be set after the
 * worker has been moved to the other thread and the thread started.
 * 
 * @param float octives : The octives parameter.
 * @param float frequency : The frequency parameter.
 * @param float persistence : The persistence parameter.
 * @param QVector3D offset : The offset parameter.
 * @param IntensityMap* height_map : The height_map parameter.
 */
void SimplexNoiseWorker::set(float octives,
                             float frequency,
                             float persistence,
                             QVector3D offset,
                             IntensityMap *height_map)
{
    this->_octives = octives;
    this->_frequency = frequency;
    this->_persistence = persistence;
    this->_offset = offset;
    this->_height_map = height_map;
}

/**
 * generate @slot
 * 
 * Generates the simplex noise in the separate thread.
 * 
 * @signals started
 * @signals progress
 * @signals done
 */
void SimplexNoiseWorker::generate()
{
    emit this->started();
    this->_run = true;
    SimplexNoise noise(this->_frequency / 1000.0f,
                       1.0f,
                       1.99f,
                       this->_persistence);
    // Create a vector map to house information
    Q_CHECK_PTR(SETTINGS);
    
    int size;

    // Ratio lets us generate a simplex map for preview that accurately
    // represents the final render version
    float ratio = 1.0f;
    if (SETTINGS->renderMode())
    {
        size = SETTINGS->renderResolution();
    }
    else
    {
        size = SETTINGS->previewResolution();
        int render_size = SETTINGS->renderResolution();
        ratio = render_size / (float)size;
    }

    int i = 0;
    int total = size * size;

    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            // Get a noise value for a specific point
            float intensity = noise.fractal(
                this->_octives,
                (float)x * ratio + this->_offset.x() * 25.0f,
                (float)y * ratio + this->_offset.y() * 25.0f,
                this->_offset.z() * 25.0f);
            // Normalize intensity from [-1, 1] -> [0, 1]
            intensity = (intensity + 1.0f) / 2.0f;
            // Save noise in vector map
            this->_height_map->append(intensity);
            emit this->progress((int)round(100.0f * (float)i / (float)total));
            i++;
            if (!this->_run)
            {
                emit this->stopped();
                return;
            }
        }
    }
    emit this->done();
}

/**
 * stop @slot
 * 
 * Sets the run flag to stop to interupt any further work during generate
 */
void SimplexNoiseWorker::stop()
{
    if (!this->_run)
        emit this->stopped();
    this->_run = false;
}

/******************************************************************************
 *                                  NODE                                      *
 ******************************************************************************/

/**
 * InputSimplexNoiseNode
 * 
 * Creates the node and created the UI.
 */
InputSimplexNoiseNode::InputSimplexNoiseNode()
{
    qDebug("Create Input Simplex Noise Node and UI widgets");
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();

    this->_ui.setupUi(this->_widget);
    this->_widget->setMinimumSize(281, 302);
    this->_shared_ui.setupUi(this->_shared_widget);
}

/**
 * ~InputSimplexNoiseNode
 * 
 * Delets the simplex noise, safely shutting down the worker thread.
 */
InputSimplexNoiseNode::~InputSimplexNoiseNode()
{
    this->_thread.quit();
    this->_thread.wait();
    delete this->_worker;
}

/**
 * created
 * 
 * Function is called when the node is created so it can connect to listeners.
 */
void InputSimplexNoiseNode::created()
{
    // Attach listeners
    QObject::connect(this->_ui.spin_octives,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &InputSimplexNoiseNode::octivesChanged);

    QObject::connect(this->_ui.spin_frequency,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &InputSimplexNoiseNode::frequencyChanged);

    QObject::connect(this->_ui.spin_persistence,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &InputSimplexNoiseNode::persistenceChanged);

    QObject::connect(this->_ui.spin_x,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &InputSimplexNoiseNode::xChanged);

    QObject::connect(this->_ui.spin_y,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &InputSimplexNoiseNode::yChanged);

    QObject::connect(this->_ui.spin_z,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &InputSimplexNoiseNode::zChanged);

    QObject::connect(this->_shared_ui.spin_octives,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &InputSimplexNoiseNode::octivesChanged);

    QObject::connect(this->_shared_ui.spin_frequency,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &InputSimplexNoiseNode::frequencyChanged);

    QObject::connect(this->_shared_ui.spin_persistence,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &InputSimplexNoiseNode::persistenceChanged);

    QObject::connect(this->_shared_ui.spin_x,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &InputSimplexNoiseNode::xChanged);

    QObject::connect(this->_shared_ui.spin_y,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &InputSimplexNoiseNode::yChanged);

    QObject::connect(this->_shared_ui.spin_z,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this,
                     &InputSimplexNoiseNode::zChanged);

    Q_CHECK_PTR(SETTINGS);
    // Settings listener
    QObject::connect(SETTINGS,
                     &Settings::previewResolutionChanged,
                     this,
                     &InputSimplexNoiseNode::_generate);
    QObject::connect(SETTINGS, &Settings::renderResolutionChanged, [this]() {
        qDebug("Render Resolution Changed");
        this->_generate();
    });
    QObject::connect(SETTINGS,
                     &Settings::renderModeChanged,
                     this,
                     &InputSimplexNoiseNode::_generate);

    // Generate values
    this->_generate();
}

/**
 * caption
 * 
 * Return a string that is displayed on the node and in the properties.
 * 
 * @returns QString : The caption.
 */
QString InputSimplexNoiseNode::caption() const
{
    return "Generate Simplex Noise Texture";
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString InputSimplexNoiseNode::name() const
{
    return "Simplex Noise Texture";
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram.
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *InputSimplexNoiseNode::embeddedWidget()
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
QWidget *InputSimplexNoiseNode::sharedWidget()
{
    Q_CHECK_PTR(this->_shared_widget);
    return this->_shared_widget;
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
unsigned int InputSimplexNoiseNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::Out ? 1 : 4;
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
InputSimplexNoiseNode::dataType(QtNodes::PortType port_type,
                                QtNodes::PortIndex port_index) const
{
    if (port_type == QtNodes::PortType::Out)
        return IntensityMapData().type();

    QtNodes::NodeDataType i = IntensityMapData().type();
    QtNodes::NodeDataType v = VectorMapData().type();

    switch ((int)port_index)
    {
    case 0:
        return {i.id, "octives"};
        break;
    case 1:
        return {i.id, "frequency"};
        break;
    case 2:
        return {i.id, "persistence"};
        break;
    case 3:
        return {v.id, "offset"};
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
    Q_UNREACHABLE();
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
InputSimplexNoiseNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return std::make_shared<IntensityMapData>(this->_intensity_map);
}

/**
 * _generate
 * 
 * Generates the output data from the supplied and available data.
 */
void InputSimplexNoiseNode::_generate()
{
    qDebug("Generating Noise Map");

    float octives = this->_octives;
    float frequency = this->_frequency;
    float persistence = this->_persistence;
    QVector3D offset = this->_offset;

    if (this->_in_octives_set)
    {
        Q_CHECK_PTR(this->_in_octives);
        octives = this->_in_octives->intensityMap().at(0, 0);
    }

    if (this->_in_frequency_set)
    {
        Q_CHECK_PTR(this->_in_frequency);
        frequency = this->_in_frequency->intensityMap().at(0, 0);
    }

    if (this->_in_persistence_set)
    {
        Q_CHECK_PTR(this->_in_persistence);
        persistence = this->_in_persistence->intensityMap().at(0, 0);
    }

    if (this->_in_offset_set)
    {
        Q_CHECK_PTR(this->_in_offset);
        glm::dvec4 val = this->_in_offset->vectorMap().at(0, 0);
        offset = QVector3D(val.x, val.y, val.z);
    }

    Q_CHECK_PTR(SETTINGS);
    int size;
    // float ratio = 1.0f;
    if (SETTINGS->renderMode())
        size = SETTINGS->renderResolution();
    else
        size = SETTINGS->previewResolution();
    // Ratio compensates the preview resolution to better reflect the render output
    this->_intensity_map = IntensityMap(size, size);

    if (!this->_worker)
    {
        this->_worker = new SimplexNoiseWorker();
        this->_worker->moveToThread(&this->_thread);

        QObject::connect(this->_worker,
                        &SimplexNoiseWorker::done,
                        this,
                        &InputSimplexNoiseNode::simplexDone);

        QObject::connect(this->_worker, &SimplexNoiseWorker::started, [this]() {
            Q_CHECK_PTR(SETTINGS);
            
            this->_ui.progress->show();
            this->_shared_ui.progress->show();

            if (SETTINGS->percentProgressText()
                && !this->_ui.progress->isTextVisible())
                this->_ui.progress->setTextVisible(true);
            else if (!SETTINGS->percentProgressText()
                    && this->_ui.progress->isTextVisible())
                this->_ui.progress->setTextVisible(false);

            if (SETTINGS->percentProgressText()
                && !this->_shared_ui.progress->isTextVisible())
                this->_shared_ui.progress->setTextVisible(true);
            else if (!SETTINGS->percentProgressText()
                    && this->_shared_ui.progress->isTextVisible())
                this->_shared_ui.progress->setTextVisible(false);
        });
        QObject::connect(this->_worker,
                        &SimplexNoiseWorker::progress,
                        this->_ui.progress,
                        &QProgressBar::setValue);

        QObject::connect(this->_worker,
                        &SimplexNoiseWorker::progress,
                        this->_shared_ui.progress,
                        &QProgressBar::setValue);

        QObject::connect(this->_worker,
                         &SimplexNoiseWorker::stopped,
                         [this]() {
                             this->_ui.progress->hide();
                             this->_shared_ui.progress->hide();
                             this->_generate();
                         });

        this->_thread.start();
    }

    this->_worker->set(octives,
                       frequency,
                       persistence,
                       offset,
                       &this->_intensity_map);

    QMetaObject::invokeMethod(this->_worker, "generate", Qt::QueuedConnection);
}

/**
 * simplexDone @slot
 * 
 * Called when the generation of the simplex noise in the other thread is done.
 * 
 * @signals dataUpdated
 */
void InputSimplexNoiseNode::simplexDone()
{
    qDebug() << "Done";
    this->_ui.progress->hide();
    this->_shared_ui.progress->hide();

    this->_output = this->_intensity_map.toPixmap();
    this->_ui.label_pixmap->setPixmap(
        this->_output.scaled(
            this->_ui.label_pixmap->width(),
            100,
            Qt::KeepAspectRatioByExpanding));

    this->_shared_ui.label_pixmap->setPixmap(
        this->_output.scaled(
            this->_shared_ui.label_pixmap->width(),
            100,
            Qt::KeepAspectRatioByExpanding));

    emit this->dataUpdated(0);
}

/**
 * save
 * 
 * Saves the state of the node into a QJsonObject for the system to save to
 * file.
 * 
 * @returns QJsonObject : The saved state of the node.
 */
QJsonObject InputSimplexNoiseNode::save() const
{
    qDebug("Saving Simplex Noise Node");
    QJsonObject data;
    data["name"] = this->name();
    data["octives"] = this->_octives;
    data["frequency"] = this->_frequency;
    data["persistence"] = this->_persistence;

    QJsonObject offset;
    offset["x"] = this->_offset.x();
    offset["y"] = this->_offset.y();
    offset["z"] = this->_offset.z();
    data["offset"] = offset;

    return data;
}

/**
 * restore
 * 
 * Restores the state of the node from a provided json object.
 * 
 * @param QJsonObject const& data : The data to restore from.
 */
void InputSimplexNoiseNode::restore(QJsonObject const &data)
{
    qDebug("Restoring Simplex Noise Node");
    // Set storage files
    this->_octives = data["octives"].toDouble();
    this->_frequency = data["frequency"].toDouble();
    this->_persistence = data["persistence"].toDouble();

    this->_offset = QVector3D(data["offset"]["x"].toDouble(),
                              data["offset"]["y"].toDouble(),
                              data["offset"]["z"].toDouble());

    // Update ui
    this->_ui.spin_octives->setValue(this->_octives);
    this->_ui.spin_frequency->setValue(this->_frequency);
    this->_ui.spin_persistence->setValue(this->_persistence);
    this->_ui.spin_x->setValue(this->_offset.x());
    this->_ui.spin_y->setValue(this->_offset.y());
    this->_ui.spin_z->setValue(this->_offset.z());

    this->_shared_ui.spin_octives->setValue(this->_octives);
    this->_shared_ui.spin_frequency->setValue(this->_frequency);
    this->_shared_ui.spin_persistence->setValue(this->_persistence);
    this->_shared_ui.spin_x->setValue(this->_offset.x());
    this->_shared_ui.spin_y->setValue(this->_offset.y());
    this->_shared_ui.spin_z->setValue(this->_offset.z());

    // Regenerate the noise
    this->_generate();
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
void InputSimplexNoiseNode::setInData(
    std::shared_ptr<QtNodes::NodeData> node_data,
    QtNodes::PortIndex port)
{
    if (node_data)
    {
        switch ((int)port)
        {
        case 0:
            this->_in_octives =
                std::dynamic_pointer_cast<IntensityMapData>(node_data);

            this->_in_octives_set = true;
            this->_ui.spin_octives->setReadOnly(true);
            this->_shared_ui.spin_octives->setReadOnly(true);
            break;
        case 1:
            this->_in_frequency =
                std::dynamic_pointer_cast<IntensityMapData>(node_data);

            this->_in_frequency_set = true;
            this->_ui.spin_frequency->setReadOnly(true);
            this->_shared_ui.spin_frequency->setReadOnly(true);
            break;
        case 2:
            this->_in_persistence =
                std::dynamic_pointer_cast<IntensityMapData>(node_data);

            this->_in_persistence_set = true;
            this->_ui.spin_persistence->setReadOnly(true);
            this->_shared_ui.spin_persistence->setReadOnly(true);
            break;
        case 3:
            this->_in_offset =
                std::dynamic_pointer_cast<VectorMapData>(node_data);

            this->_in_offset_set = true;

            this->_ui.spin_x->setReadOnly(true);
            this->_ui.spin_y->setReadOnly(true);
            this->_ui.spin_z->setReadOnly(true);

            this->_shared_ui.spin_x->setReadOnly(true);
            this->_shared_ui.spin_y->setReadOnly(true);
            this->_shared_ui.spin_z->setReadOnly(true);
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
        this->_generate();
    }
}

/**
 * inputConnectionDeleted @slot
 * 
 * Called when an input connection is deleted, this usually resets some data and
 * regenerates the output data.
 * 
 * @param QtNodes::Connection const& connection : The connection being deleted.
 */
void InputSimplexNoiseNode::inputConnectionDeleted(
    QtNodes::Connection const &connection)
{
    int port = (int)connection.getPortIndex(QtNodes::PortType::In);
    switch (port)
    {
    case 0:
        this->_in_octives_set = false;
        this->_ui.spin_octives->setReadOnly(false);
        this->_shared_ui.spin_octives->setReadOnly(false);
        break;
    case 1:
        this->_in_frequency_set = false;
        this->_ui.spin_frequency->setReadOnly(false);
        this->_shared_ui.spin_frequency->setReadOnly(false);
        break;
    case 2:
        this->_in_persistence_set = false;
        this->_ui.spin_persistence->setReadOnly(false);
        this->_shared_ui.spin_persistence->setReadOnly(false);
        break;
    case 3:
        this->_in_offset_set = false;

        this->_ui.spin_x->setReadOnly(false);
        this->_ui.spin_y->setReadOnly(false);
        this->_ui.spin_z->setReadOnly(false);

        this->_shared_ui.spin_x->setReadOnly(false);
        this->_shared_ui.spin_y->setReadOnly(false);
        this->_shared_ui.spin_z->setReadOnly(false);
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
    this->_generate();
}

/**
 * octivesChanged
 * 
 * Called when the control widget changes the octives. Updates values and
 * regenerates the simplex noise.
 * 
 * @param double value : The new value.
 */
void InputSimplexNoiseNode::octivesChanged(double value)
{
    this->_octives = (float)value;
    this->_ui.spin_octives->setValue(this->_octives);
    this->_shared_ui.spin_octives->setValue(this->_octives);
    if (this->_worker)
        QMetaObject::invokeMethod(this->_worker, "stop", Qt::QueuedConnection);
    else
        this->_generate();
}

/**
 * frequencyChanged
 * 
 * Called when the control widget changes the frequency. Updates values and
 * regenerates the simplex noise.
 * 
 * @param double value : The new value.
 */
void InputSimplexNoiseNode::frequencyChanged(double value)
{
    this->_frequency = (float)value;
    this->_ui.spin_frequency->setValue(this->_frequency);
    this->_shared_ui.spin_frequency->setValue(this->_frequency);
    if (this->_worker)
        QMetaObject::invokeMethod(this->_worker, "stop", Qt::QueuedConnection);
    else
        this->_generate();
}

/**
 * persistenceChanged
 * 
 * Called when the control widget changes the persistence. Updates values and
 * regenerates the simplex noise.
 * 
 * @param double value : The new value.
 */
void InputSimplexNoiseNode::persistenceChanged(double value)
{
    this->_persistence = (float)value;
    this->_ui.spin_persistence->setValue(this->_persistence);
    this->_shared_ui.spin_persistence->setValue(this->_persistence);
    if (this->_worker)
        QMetaObject::invokeMethod(this->_worker, "stop", Qt::QueuedConnection);
    else
        this->_generate();
}

/**
 * xChanged
 * 
 * Called when the control widget changes the x. Updates values and
 * regenerates the simplex noise. updates the offset.
 * 
 * @param double value : The new value.
 */
void InputSimplexNoiseNode::xChanged(double value)
{
    this->_offset.setX((float)value);
    this->_ui.spin_x->setValue(this->_offset.x());
    this->_shared_ui.spin_x->setValue(this->_offset.x());
    if (this->_worker)
        QMetaObject::invokeMethod(this->_worker, "stop", Qt::QueuedConnection);
    else
        this->_generate();
}

/**
 * yChanged
 * 
 * Called when the control widget changes the y. Updates values and
 * regenerates the simplex noise. updates the offset.
 * 
 * @param double value : The new value.
 */
void InputSimplexNoiseNode::yChanged(double value)
{
    this->_offset.setY((float)value);
    this->_ui.spin_y->setValue(this->_offset.y());
    this->_shared_ui.spin_y->setValue(this->_offset.y());
    if (this->_worker)
        QMetaObject::invokeMethod(this->_worker, "stop", Qt::QueuedConnection);
    else
        this->_generate();
}

/**
 * zChanged
 * 
 * Called when the control widget changes the z. Updates values and
 * regenerates the simplex noise. updates the offset.
 * 
 * @param double value : The new value.
 */
void InputSimplexNoiseNode::zChanged(double value)
{
    this->_offset.setZ((float)value);
    this->_ui.spin_z->setValue(this->_offset.z());
    this->_shared_ui.spin_z->setValue(this->_offset.z());
    if (this->_worker)
        QMetaObject::invokeMethod(this->_worker, "stop", Qt::QueuedConnection);
    else
        this->_generate();
}
