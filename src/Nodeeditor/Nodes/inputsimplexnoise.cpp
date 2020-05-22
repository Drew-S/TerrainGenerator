#include "inputsimplexnoise.h"

#include "Globals/settings.h"

#include "../Datatypes/pixmap.h"

#include <QImage>
#include <QColor>
#include <QDebug>
#include <QDoubleSpinBox>

// Create a node and attach listeners
InputSimplexNoiseNode::InputSimplexNoiseNode()
{
    qDebug("Creating Input Simplex Noise Node, attaching listeners and UI widget");
    this->_widget = new QWidget();
    this->_shared_widget = new QWidget();
    this->_ui.setupUi(this->_widget);
    this->_widget->setMinimumSize(281, 302);
    this->_shared_ui.setupUi(this->_shared_widget);

    // Attach listeners
    QObject::connect(this->_ui.spin_octives, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::octivesChanged);
    QObject::connect(this->_ui.spin_frequency, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::frequencyChanged);
    QObject::connect(this->_ui.spin_persistence, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::persistenceChanged);
    QObject::connect(this->_ui.spin_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::xChanged);
    QObject::connect(this->_ui.spin_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::yChanged);
    QObject::connect(this->_ui.spin_z, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::zChanged);

    QObject::connect(this->_shared_ui.spin_octives, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::octivesChanged);
    QObject::connect(this->_shared_ui.spin_frequency, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::frequencyChanged);
    QObject::connect(this->_shared_ui.spin_persistence, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::persistenceChanged);
    QObject::connect(this->_shared_ui.spin_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::xChanged);
    QObject::connect(this->_shared_ui.spin_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::yChanged);
    QObject::connect(this->_shared_ui.spin_z, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::zChanged);

    Q_CHECK_PTR(SETTINGS);
    // Settings listener
    QObject::connect(SETTINGS, &Settings::previewResolutionChanged, this, &InputSimplexNoiseNode::_generate);
    QObject::connect(SETTINGS, &Settings::renderResolutionChanged, this, &InputSimplexNoiseNode::_generate);
    QObject::connect(SETTINGS, &Settings::renderModeChanged, this, &InputSimplexNoiseNode::_generate);

    // Generate values
    this->_generate();
}

InputSimplexNoiseNode::~InputSimplexNoiseNode() {}

// Title shown at the top of the node
QString InputSimplexNoiseNode::caption() const
{
    return "Generate Simplex Noise Texture";
}

// Title shown in the selection list
QString InputSimplexNoiseNode::name() const
{
    return "Simplex Noise Texture";
}

// The image label that is embedded in the node
QWidget *InputSimplexNoiseNode::embeddedWidget()
{
    Q_CHECK_PTR(this->_widget);
    return this->_widget;
}
QWidget *InputSimplexNoiseNode::sharedWidget()
{
    Q_CHECK_PTR(this->_shared_widget);
    return this->_shared_widget;
}

// Get the number of in and out ports
unsigned int InputSimplexNoiseNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::Out ? 1 : 0;
}

// Get the data type for the port inputs and output
QtNodes::NodeDataType InputSimplexNoiseNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    return IntensityMapData().type();
}

// Get the data attached to a port
std::shared_ptr<QtNodes::NodeData> InputSimplexNoiseNode::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    return std::make_shared<IntensityMapData>(this->_intensity_map);
}

// Generates the noise texture
void InputSimplexNoiseNode::_generate()
{
    qDebug("Generating Noise Map");
    // Create a simplex noise generator with parameters
    this->_noise = SimplexNoise(this->_frequency / 1000.0f, 1.0f, 1.99f, this->_persistence);

    // Create a vector map to house information
    Q_CHECK_PTR(SETTINGS);
    int size;
    if (SETTINGS->renderMode())
        size = SETTINGS->renderResolution();
    else
        size = SETTINGS->previewResolution();
    int render_size = SETTINGS->renderResolution();
    // Ratio compensates the preview resolution to better reflect the render output
    double ratio = size / (double)render_size;
    this->_intensity_map = IntensityMap(size, size);
    // Generate the "image" with the specified size
    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            // Get a noise value for a specific point
            float intensity = this->_noise.fractal(
                this->_octives,
                (float)x * ratio + this->_offset.x() * 25.0f,
                (float)y * ratio + this->_offset.y() * 25.0f,
                this->_offset.z() * 25.0f);
            // Normalize intensity from [-1, 1] -> [0, 1]
            intensity = (intensity + 1.0f) / 2.0f;
            // Save noise in vector map
            this->_intensity_map.append(intensity);
        }
    }

    // Set preview and emit completion of generation
    this->_pixmap = this->_intensity_map.toPixmap();
    this->_ui.label_pixmap->setPixmap(this->_pixmap.scaled(this->_ui.label_pixmap->width(), 100, Qt::KeepAspectRatioByExpanding));
    this->_shared_ui.label_pixmap->setPixmap(this->_pixmap.scaled(this->_shared_ui.label_pixmap->width(), 100, Qt::KeepAspectRatioByExpanding));
    emit this->dataUpdated(0);
}

// Save the node setup to a Json Object for saving to a file
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

// Restore the nodes setup from a save file (json object)
void InputSimplexNoiseNode::restore(QJsonObject const &data)
{
    qDebug("Restoring Simplex Noise Node");
    // Set storage files
    this->_octives = data["octives"].toDouble();
    this->_frequency = data["frequency"].toDouble();
    this->_persistence = data["persistence"].toDouble();

    this->_offset = QVector3D(data["offset"]["x"].toDouble(), data["offset"]["y"].toDouble(), data["offset"]["z"].toDouble());

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

// Sets data from an input port, not yet implemented
void InputSimplexNoiseNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
{
    Q_UNUSED(node_data);
    Q_UNUSED(port);
}

// Listeners for updating generation parameters and re-running generation
void InputSimplexNoiseNode::octivesChanged(double value)
{
    this->_octives = (float)value;
    this->_ui.spin_octives->setValue(this->_octives);
    this->_shared_ui.spin_octives->setValue(this->_octives);
    this->_generate();
}
void InputSimplexNoiseNode::frequencyChanged(double value)
{
    this->_frequency = (float)value;
    this->_ui.spin_frequency->setValue(this->_frequency);
    this->_shared_ui.spin_frequency->setValue(this->_frequency);
    this->_generate();
}
void InputSimplexNoiseNode::persistenceChanged(double value)
{
    this->_persistence = (float)value;
    this->_ui.spin_persistence->setValue(this->_persistence);
    this->_shared_ui.spin_persistence->setValue(this->_persistence);
    this->_generate();
}
void InputSimplexNoiseNode::xChanged(double value)
{
    this->_offset.setX((float)value);
    this->_ui.spin_x->setValue(this->_offset.x());
    this->_shared_ui.spin_x->setValue(this->_offset.x());
    this->_generate();
}
void InputSimplexNoiseNode::yChanged(double value)
{
    this->_offset.setY((float)value);
    this->_ui.spin_y->setValue(this->_offset.y());
    this->_shared_ui.spin_y->setValue(this->_offset.y());
    this->_generate();
}
void InputSimplexNoiseNode::zChanged(double value)
{
    this->_offset.setZ((float)value);
    this->_ui.spin_z->setValue(this->_offset.z());
    this->_shared_ui.spin_z->setValue(this->_offset.z());
    this->_generate();
}
