/**
 * Mei, X., Decaudin, P., & Hu, B.-G. (2007). Fast Hydraulic Erosion Simulation
 *        and Visualization on GPU. 15th Pacific Conference on Computer Graphics
 *        and Applications (PG’07), Computer Graphics and Applications, 2007.
 *        PG ’07. 15th Pacific Conference On, 47–56.
 *        https://doi.org/10.1109/PG.2007.15
 */
#include "erosion.h"

#include <math.h>
#include <random>

#include <QDebug>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSpinBox>
#include <QThread>

#include <glm/vec2.hpp>

/**
 * mix
 * 
 * Interpolates a value between a range using a percentage value.
 * 
 * @param double x : The start value.
 * @param double y : The end value.
 * @param double a : The percentage value between x and y.
 * 
 * @returns double : The interpolated value.
 */
static double mix(double x, double y, double a)
{
    return x * (1.00 - a) + y * a;
}

/**
 * biLinearMix
 * 
 * Bilinear interpolation between four values and two percentages. Applies 
 * mix(a, b, x), mix(c, d, x) and then mixes the result with y. See above mix
 * function. Used for interpolating a value in between four pixels.
 * 
 * @param double a : The first value to use (a - b).
 * @param double b : The second value to use (a - b).
 * @param double c : The third value to use (c - d).
 * @param double d : The fourth value to use (c - d).
 * @param double x : The interpolation percentage for (a - b) and (c - d).
 * @param double y : The interpolation percentage for the result from
 *                   interpolating (a - b) and (c - d).
 * 
 * @returns double : The resulting interpolated value.
 */
static double biLinearMix(double a,
                          double b,
                          double c,
                          double d,
                          double x,
                          double y)
{
    return mix(mix(a, b, x), mix(c, d, x), y);
}

/**
 * height
 * 
 * Get the height at a position on the map. Uses bilinear interpolation to get
 * the value for between pixels.
 * 
 * @param IntensityMap *map : The map to get value from.
 * @param double x : The x position to get.
 * @param double y : The y position to get.
 * 
 * @returns double : The interpolated height
 */
static double height(IntensityMap *map, double x, double y)
{
    int x_0 = (int)floor(x);
    int y_0 = (int)floor(y);

    double xp = x - floor(x);
    double yp = y - floor(y);

    return biLinearMix(map->at(x_0, y_0, true),
                       map->at(x_0 + 1, y_0, true),
                       map->at(x_0, y_0 + 1, true),
                       map->at(x_0 + 1, y_0 + 1, true),
                       xp, yp);
}

/**
 * interpolateAdd
 * 
 * Helper function that adds a value to a map using interpolation for values
 * between pixels.
 * 
 * @param IntensityMap *map : The map to affect.
 * @param double x : The x position to affect.
 * @param double y : The y position to affect.
 * @param double v : The value to place into the map.
 */
static void interpolateAdd(IntensityMap *map, double x, double y, double v)
{
    int x_0 = (int)floor(x);
    int y_0 = (int)floor(y);

    double xp = x - floor(x);
    double yp = y - floor(y);
    double xp_1 = 1.00 - xp;
    double yp_1 = 1.00 - yp;

    map->set(x_0, y_0, map->at(x_0, y_0, true) + v * xp_1 * yp_1);
    map->set(x_0 + 1, y_0, map->at(x_0 + 1, y_0, true) + v * xp * yp_1);
    map->set(x_0, y_0 + 1, map->at(x_0, y_0 + 1, true) + v * xp_1 * yp);
    map->set(x_0 + 1, y_0 + 1, map->at(x_0 + 1, y_0 + 1, true) + v * xp * yp);
}

/**
 * clamp
 * 
 * Clamps an input value between a low and high value. f(x) -> l <= x <= h
 * 
 * @param int x : The value to clamp.
 * @param int l : The low value.
 * @param int h : The high value.
 * 
 * @returns int : The clamped value.
 */
static int clamp(int x, int l, int h)
{
    return std::min(h, std::max(l, x));
}

/**
 * gradient
 * 
 * Obtain the direction (x, y) of greatest descent, the -gradient. The value is
 * calculated using linear interpolation to get appropriate heights between
 * pixels.
 * 
 * @param IntensityMap *map : The height map.
 * @param double x : The x position to get
 * @param double y : The y position to get
 * 
 * @returns glm::dvec2 : The direction of greatest descent
 */
static glm::dvec2 gradient(IntensityMap *map, double x, double y)
{
    int x_0 = (int)floor(x);
    int y_0 = (int)floor(y);

    double xp = x - floor(x);
    double yp = y - floor(y);

    // Interpolate left/right positions
    double r = mix(map->at(x_0 + 1, y_0 - 1), map->at(x_0 + 1, y_0 + 1), yp);
    double l = mix(map->at(x_0 - 1, y_0 - 1), map->at(x_0 - 1, y_0 + 1), yp);

    // Interpolate top/bottom positions
    double t = mix(map->at(x_0 - 1, y_0 + 1), map->at(x_0 - 1, y_0 + 1), xp);
    double b = mix(map->at(x_0 - 1, y_0 - 1), map->at(x_0 - 1, y_0 - 1), xp);

    // Get the descent
    return glm::dvec2( 0.5 * (l - r), 0.5 * (t - b));
}

/**
 * ConverterErosionNode
 * 
 * Creates the node.
 */
ConverterErosionNode::ConverterErosionNode()
{
    this->_widget = new QWidget();
    this->_ui.setupUi(this->_widget);
}

/**
 * created
 * 
 * Function is called when the node is created so it can connect to listeners.
 */
void ConverterErosionNode::created()
{
    QObject::connect(this->_ui.run_btn,
                     &QPushButton::clicked,
                     this,
                     &ConverterErosionNode::_generate);

    QObject::connect(this->_ui.spin_inertia,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value) {
                         this->_inertia = value;
                     });

    QObject::connect(this->_ui.spin_radius,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value) {
                         this->_erosion_radius = value;
                     });

    QObject::connect(this->_ui.spin_evaporation,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value) {
                         this->_evaporation_rate = value;
                     });

    QObject::connect(this->_ui.spin_smooth,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value) {
                         this->_smooth_strength = value;
                     });

    QObject::connect(this->_ui.spin_iterations,
                     QOverload<int>::of(&QSpinBox::valueChanged),
                     [this](int value) {
                         this->_iterations = value;
                     });

    QObject::connect(this->_ui.spin_life,
                     QOverload<int>::of(&QSpinBox::valueChanged),
                     [this](int value) {
                         this->_max_drop_life = value;
                     });
}

/**
 * caption
 * 
 * Return a string that is displayed on the node and in the properties.
 * 
 * @returns QString : The caption.
 */
QString ConverterErosionNode::caption() const
{
    return QString("Erosion");
}

/**
 * name
 * 
 * Return a string that is displayed in the node selection list.
 * 
 * @returns QString : The name.
 */
QString ConverterErosionNode::name() const
{
    return QString("Erosion");
}

/**
 * embeddedWidget
 * 
 * Returns a pointer to the widget that gets embedded within the node in the
 * dataflow diagram.
 * 
 * @returns QWidget* : The embedded widget.
 */
QWidget *ConverterErosionNode::embeddedWidget()
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
QWidget *ConverterErosionNode::sharedWidget()
{
    return nullptr;
    // Q_CHECK_PTR(this->_shared_widget);
    // return this->_shared_widget;
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
unsigned int ConverterErosionNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::In ? 1 : 3;
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
ConverterErosionNode::dataType(QtNodes::PortType port_type,
                            QtNodes::PortIndex port_index) const
{
    if (port_type == QtNodes::PortType::In)
        return IntensityMapData().type();

    QtNodes::NodeDataType type = IntensityMapData().type();

    switch((int)port_index)
    {
        case 0:
            return QtNodes::NodeDataType{type.id, "Height"};
            break;
        case 1:
            return QtNodes::NodeDataType{type.id, "Sediment"};
            break;
        case 2:
            return QtNodes::NodeDataType{type.id, "Erosion"};
            break;
        default:
            Q_UNREACHABLE();
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
void ConverterErosionNode::setInData(
    std::shared_ptr<QtNodes::NodeData> node_data,
    QtNodes::PortIndex port)
{
    Q_UNUSED(port);
    if (node_data)
    {
        this->_input = std::dynamic_pointer_cast<IntensityMapData>(node_data);
        this->_set = true;

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
void ConverterErosionNode::inputConnectionDeleted(
    QtNodes::Connection const &connection)
{
    Q_UNUSED(connection);
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
QJsonObject ConverterErosionNode::save() const
{
    qDebug("Saving erosion node");
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
void ConverterErosionNode::restore(QJsonObject const &data)
{
    Q_UNUSED(data);
    qDebug("Restoring erosion node");
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
ConverterErosionNode::outData(QtNodes::PortIndex port)
{
    switch((int)port)
    {
    case 0:
        return std::make_shared<IntensityMapData>(this->_output);
        break;
    case 1:
        return std::make_shared<IntensityMapData>(this->_sediment);
        break;
    case 2:
        return std::make_shared<IntensityMapData>(this->_erosion);
        break;
    default:
        Q_UNREACHABLE();
    }
    Q_UNREACHABLE();
}

/**
 * _smooth
 * 
 * Applies a smoothing function (kernel) to a pixel at a specific point.
 * Used during the sediment placement process as there is some heavy
 * artifacting. Interesting side effect of this function the way it is, is the
 * changing of the smooth value effects an intensity of the erosion effect.
 * 
 * @param int x : The x pixel to smooth.
 * @param int y : The y pixel to smooth.
 */
void ConverterErosionNode::_smooth(int x, int y)
{
    // Simple smoothing kernel
    static double K[3][3] = {
        {1.00, 4.00, 1.00},
        {4.00, this->_smooth_strength + 20.00, 4.00},
        {1.00, 4.00, 1.00}};

    // Averaging value
    double div = this->_smooth_strength + 40.00;

    // Compensation for edge values
    if (x <= 0 || x >= this->_output.width - 1)
        div -= 6.00;

    if (y <= 0 || y >= this->_output.height - 1)
        div -= 6.00;

    if ((x <= 0 || x >= this->_output.width - 1)
        && (y <= 0 || y >= this->_output.height - 1))
        div += 1.00;

    // Get pixels to smooth with
    double M[3][3] = {
        {this->_output.at(x - 1, y - 1),
         this->_output.at(x, y - 1),
         this->_output.at(x + 1, y - 1)},
        {this->_output.at(x - 1, y),
         this->_output.at(x, y),
         this->_output.at(x + 1, y)},
        {this->_output.at(x - 1, y + 1),
         this->_output.at(x, y + 1),
         this->_output.at(x + 1, y + 1)}};

    // Apply smoothing
    double v = M[0][0] * K[0][0] + M[1][0] * K[1][0] + M[2][0] * K[2][0]
             + M[0][1] * K[0][1] + M[1][1] * K[1][1] + M[2][1] * K[2][1]
             + M[0][2] * K[0][2] + M[1][2] * K[1][2] + M[2][2] * K[2][2];

    this->_output.set(x, y, v / div);
}

/**
 * _generate
 * 
 * Generates the output data from the supplied and available data.
 * 
 * @signals dataUpdated
 */
void ConverterErosionNode::_generate()
{
    Q_CHECK_PTR(this->_input);
    this->_output = this->_input->intensityMap();

    this->_sediment =
        IntensityMap(this->_output.width, this->_output.height, 0.00);
        
    this->_erosion =
        IntensityMap(this->_output.width, this->_output.height, 0.00);

    // Generate a random distribution of rain drops
    std::default_random_engine gen;

    std::uniform_real_distribution<double>
        distrib_x(0.00, (double)this->_output.width - 1.00);

    std::uniform_real_distribution<double>
        distrib_y(0.00, (double)this->_output.height - 1.00);

    // Erosion radius distance, for rounding erosion distance from point
    double r = sqrt(2.00 * this->_erosion_radius * this->_erosion_radius);

    // Generate _iterations number of rain drops
    for (int i = 0; i < this->_iterations; i++)
    {
        // Setup default values
        double dir_x = 0.00;
        double dir_y = 0.00;

        // Random raindrop location
        // TODO: Add a rainmap (intensity map) that affects water value
        double pos_x = distrib_x(gen);
        double pos_y = distrib_y(gen);

        double speed = 1.00;
        double sediment = 0.00;
        double water = 1.00;

        // While the raindrop lives, move, erode, and deposit.
        for (int l = 0; l < this->_max_drop_life; l++)
        {
            // Get the current height
            double old_height = height(&this->_output, pos_x, pos_y);

            // Get the greatest descent
            glm::dvec2 grad = gradient(&this->_output, pos_x, pos_y);

            // Update the movement direction with inertia and gradient descent
            dir_x = dir_x * this->_inertia
                    - grad.x * (1.00 - this->_inertia);

            dir_y = dir_y * this->_inertia
                    - grad.y * (1.00 - this->_inertia);

            // Normalize the movement (1 pixel radius distance of movement)
            double norm = sqrt(dir_x * dir_x + dir_y * dir_y);
            if (norm != 0.00)
            {
                dir_x /= norm;
                dir_y /= norm;
            }

            // Update particle position
            pos_x += dir_x;
            pos_y += dir_y;

            // Fell off map or stopped moving? Go to next particle
            if ((dir_x == 0.00 && dir_y == 0.00)
                || pos_x < 0.00
                || pos_x > (double)this->_output.width - 1.00
                || pos_y < 0.00
                || pos_y > (double)this->_output.height - 1.00)
                break;

            // Get new position height
            double new_height = height(&this->_output, pos_x, pos_y);

            // Get the change in height
            double delta_height = new_height - old_height;

            // Calculate the sediment capacity
            double sediment_cap =
                std::max(-delta_height * speed * water
                         * this->_sediment_capacity,
                         this->_min_sediment_capacity);

            // If sediment is beyond capacity deposit sediment
            if (sediment > sediment_cap
                || delta_height > 0.00
                || (dir_x == 0.00 && dir_y == 0.00))
            {
                // Deposit at most the change in height (prevents holes)
                double deposit = delta_height > 0.00
                                 ? std::min(delta_height, sediment)
                                 : (sediment - sediment_cap)
                                   * this->_deposit_speed;

                // Update the sediment value
                sediment -= deposit;
                    
                // Place the depositing value onto the map
                interpolateAdd(&this->_output, pos_x, pos_y, deposit);
                interpolateAdd(&this->_sediment, pos_x, pos_y, deposit);

                // Smooth particle (apply intensity)
                this->_smooth((int)round(pos_x), (int)round(pos_y));
            }

            // Still have capacity for sediment, erode some terrain
            else
            {
                // Get erosion amount
                double erode = std::min((sediment_cap - sediment)
                                        * this->_erosion_speed,
                                        -delta_height);

                // Erode pixels in a radius from position
                for (double x = -this->_erosion_radius;
                     x <= this->_erosion_radius;
                     x += 1.00)
                {
                    for (double y = -this->_erosion_radius;
                         y <= this->_erosion_radius;
                         y += 1.00)
                    {
                        // Reduce erosion the farther away from the point
                        double strength = std::min(0.001,
                                            (r - sqrt(x * x + y * y))
                                            / r * erode);

                        // Update the terrain with erosion
                        interpolateAdd(&this->_output,
                                       pos_x + x,
                                       pos_y + y,
                                       -strength);

                        interpolateAdd(&this->_erosion,
                                       pos_x + x,
                                       pos_y + y,
                                       -strength);
                        
                        // Normally would be 'sediment += strength' however,
                        // there is an odd problem where sediment is 0,
                        // strength is some positive number, but sediment
                        // becomes -strength
                        sediment = abs(sediment + strength);
                    }
                }
            }
            // Update the speed of the droplet
            speed = sqrt(speed * speed + delta_height * this->_g);
            // Slowly evaporate the water
            water *= (1.00 - this->_evaporation_rate);
        }
    }
    emit this->dataUpdated(0);
}