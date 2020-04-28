#include "inputsimplexnoise.h"

#include "../Datatypes/pixmap.h"

#include <QImage>
#include <QColor>
#include <QDebug>
#include <QDoubleSpinBox>

// Create a node and attach listeners
InputSimplexNoiseNode::InputSimplexNoiseNode()
{
    this->_widget = new QWidget();
    this->_ui.setupUi(this->_widget);

    // Attach listeners
    QObject::connect(this->_ui.spin_octives, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::octivesChanged);
    QObject::connect(this->_ui.spin_frequency, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::frequencyChanged);
    QObject::connect(this->_ui.spin_persistence, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::persistenceChanged);
    QObject::connect(this->_ui.spin_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::xChanged);
    QObject::connect(this->_ui.spin_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::yChanged);
    QObject::connect(this->_ui.spin_z, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputSimplexNoiseNode::zChanged);

    // Generate values
    this->_generate();
}

// Get the number of in and out ports
unsigned int InputSimplexNoiseNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::Out ? 1 : 0;
}

// Get the data type for the port inputs and output
QtNodes::NodeDataType InputSimplexNoiseNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    (void)port_type;
    (void)port_index;
    return PixmapData().type();
}

// Get the data attached to a port
std::shared_ptr<QtNodes::NodeData> InputSimplexNoiseNode::outData(QtNodes::PortIndex port)
{
    (void)port;
    return std::make_shared<PixmapData>(this->_color_map);
}

// Generates the noise texture
// TODO: ALL NODES, create render version of calculations for final high res generation
void InputSimplexNoiseNode::_generate()
{
    // Create a simplex noise generator with parameters
    this->_noise = SimplexNoise(this->_frequency / 1000.0f, 1.0f, 1.99f, this->_persistence);

    // Create a vector map to house information
    // TODO: Add support for preview image size control
    // TODO: Switch to use intensity map
    this->_color_map = VectorMap(256, 256);
    // Generate the "image" with the specified size
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 256; y++)
        {
            // Get a noise value for a specific point
            float intensity = this->_noise.fractal(
                this->_octives,
                (float)x + this->_offset.x() * 25.0f,
                (float)y + this->_offset.y() * 25.0f,
                this->_offset.z() * 25.0f);
            // Normalize intensity from [-1, 1] -> [0, 1]
            intensity = (intensity + 1.0f) / 2.0f;
            // Save noise in vector map
            this->_color_map.append(glm::dvec3(intensity, intensity, intensity));
        }
    }

    // Set preview and emit completion of generation
    this->_pixmap = this->_color_map.toPixmap();
    this->_ui.label_pixmap->setPixmap(this->_pixmap.scaled(100, 100, Qt::KeepAspectRatio));
    emit this->dataUpdated(0);
}

// Listeners for updating generation parameters and re-running generation
void InputSimplexNoiseNode::octivesChanged(double value)
{
    this->_octives = (float)value;
    this->_generate();
}
void InputSimplexNoiseNode::frequencyChanged(double value)
{
    this->_frequency = (float)value;
    this->_generate();
}
void InputSimplexNoiseNode::persistenceChanged(double value)
{
    this->_persistence = (float)value;
    this->_generate();
}
void InputSimplexNoiseNode::xChanged(double value)
{
    this->_offset.setX((float)value);
    this->_generate();
}
void InputSimplexNoiseNode::yChanged(double value)
{
    this->_offset.setY((float)value);
    this->_generate();
}
void InputSimplexNoiseNode::zChanged(double value)
{
    this->_offset.setZ((float)value);
    this->_generate();
}
