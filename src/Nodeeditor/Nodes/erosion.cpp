/**
 * Mei, X., Decaudin, P., & Hu, B.-G. (2007). Fast Hydraulic Erosion Simulation
 *        and Visualization on GPU. 15th Pacific Conference on Computer Graphics
 *        and Applications (PG’07), Computer Graphics and Applications, 2007.
 *        PG ’07. 15th Pacific Conference On, 47–56.
 *        https://doi.org/10.1109/PG.2007.15
 */
#include "erosion.h"

#include <math.h>

#include <QDebug>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSpinBox>
#include <QThread>

#include <glm/ext.hpp>
#include <glm/vec2.hpp>

#define LOOP_MAP(map)                    \
    for (int y = 0; y < map.height; y++) \
        for (int x = 0; x < map.width; x++)

#define COR 0.50
#define CEN 2.00
#define MID 100.00

static double K[3][3] = {
    {COR, CEN, COR},
    {CEN, MID, CEN},
    {COR, CEN, COR}};

static int clamp(int x, int l, int h)
{
    return std::min(h, std::max(l, x));
}

static double mix(double x, double y, double a)
{
    return x * (1.00 - a) + y * a;
}

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

    QObject::connect(this->_ui.spin_kc,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value) {
                         this->K_c = value;
                     });

    QObject::connect(this->_ui.spin_ks,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value) {
                         this->K_s = value;
                     });

    QObject::connect(this->_ui.spin_kd,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value) {
                         this->K_d = value;
                     });

    QObject::connect(this->_ui.spin_ke,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value) {
                         this->K_e = value;
                     });
    QObject::connect(this->_ui.spin_iterations,
                     QOverload<int>::of(&QSpinBox::valueChanged),
                     [this](int value) {
                         this->I = value;
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

void ConverterErosionNode::smooth()
{
    IntensityMap map(this->_output.width, this->_output.height);
    double L = CEN + 2.00 * COR;
    LOOP_MAP(this->_output)
    {
        if (this->w_2.at(x,y) == 0.00)
        {
            map.set(x, y, this->_output.at(x, y));
            continue;
        }

        double div = MID + 4.00 * CEN + 4.00 * COR;

        if (x <= 0 || x >= this->_output.width - 1)
            div -= L;

        if (y <= 0 || y >= this->_output.height - 1)
            div -= L;

        if ((x <= 0 || x >= this->_output.width - 1)
            && (y <= 0 || y >= this->_output.height - 1))
            div += COR;

        double M[3][3] = {
            {this->_output.at(x - 1, y - 1),
             this->_output.at(x, y - 1),
             this->_output.at(x + 1, y - 1)},
            {this->_output.at(x - 1, y),
             this->_output.at(x, y),
             this->_output.at(x + 1, y)},
            {this->_output.at(x - 1, y + 1),
             this->_output.at(x, y + 1),
             this->_output.at(x + 1, y + 1)}
        };

        double v = M[0][0] * K[0][0] + M[1][0] * K[1][0] + M[2][0] * K[2][0]
                   + M[0][1] * K[0][1] + M[1][1] * K[1][1] + M[2][1] * K[2][1]
                   + M[0][2] * K[0][2] + M[1][2] * K[1][2] + M[2][2] * K[2][2];

        map.set(x, y, v / div);
    }
    this->_output = map;
}

void ConverterErosionNode::waterIncrement1()
{
    LOOP_MAP(this->_output)
    {
        // d_1(x,y) = d_t(x,y) + \Delta t \cdot r_t(x,y)
        // (Mei et al., 2007, p. 49)
        this->w_1.set(x, y, this->w.at(x, y) + this->dT * this->r.at(x, y));
    }
}

void ConverterErosionNode::flowSimulation2()
{
    LOOP_MAP(this->_output)
    {
        // \Delta H = b_t(x,y) + d_1(x,y)
        // (Mei et al., 2007, p. 50)
        double dH = this->_output.at(x, y) + this->w_1.at(x, y);

        // \begin{aligned}
        // \Delta H^L &= b_t(x,y) + d_1(x,y) - b_t(x-1,y) - d_1(x-1,y) \\
        //            &= \Delta H - b_t(x-1,y) - d_1(x-1,y)
        // \end{aligned}
        // (Mei et al., 2007, p. 50)
        double dH_L = dH - this->_output.at(x - 1, y) - this->w_1.at(x - 1, y);
        double dH_R = dH - this->_output.at(x + 1, y) - this->w_1.at(x + 1, y);
        double dH_T = dH - this->_output.at(x, y - 1) - this->w_1.at(x, y - 1);
        double dH_B = dH - this->_output.at(x, y + 1) - this->w_1.at(x, y + 1);

        glm::dvec4 flux = this->f.at(x, y);

        // \begin{aligned}
        // p &= \Delta t \cdot A \cdot \frac{g}{l} \\
        // f_{t + \Delta t}^L(x,y) &= max(0, f_t^L(x,y) + \Delta t \cdot A \cdot
        //                            \frac{g \cdot \Delta h^L(x,y)}{l}) \\
        //                         &= max(0, f_t^L(x,y) + p \cdot h^L(x,y))
        // \end{aligneds}
        // (Mei et al., 2007, p. 50)
        double fL = std::max(0.00, flux.x + this->p * dH_L);
        double fR = std::max(0.00, flux.y + this->p * dH_R);
        double fT = std::max(0.00, flux.z + this->p * dH_T);
        double fB = std::max(0.00, flux.w + this->p * dH_B);

        // K = min(1, \frac{d_1 \cdot l_X l_Y}
        //                 {(f^L + f^R + f^T + f^B) \cdot \Delta t})
        // (Mei et al., 2007, p. 50)
        double K = std::min(1.00, (this->w_1.at(x,y) * this->dX * this->dY)
                                / ((fL + fR + fT + fB) * this->dT));

        // f_{t + \Delta t}^i = K \cdot f_{t + \Delta t}^i, \;\; i = L,R,T,B
        // (Mei et al., 2007, p. 50)
        fL *= K;
        fR *= K;
        fT *= K;
        fB *= K;

        // Boundary clamp.
        // Prevent flowing over edge of the flat earth. /s
        if (x <= 0)
            fL = 0.00;
        if (x >= this->_output.width - 1)
            fR = 0.00;
        if (y <= 0)
            fT = 0.00;
        if (y >= this->_output.height - 1)
            fB = 0.00;

        // Pack the flux into vector map
        // TODO: Convert both IntensityMap and VectorMap into a single 2DMap
        //       class using templates, thus velocity vector here can be better
        //       packed using glm::dvec2 rather than using a glm::dvec4 with z
        //       and w being unused and wasted space
        this->f.set(x, y, glm::dvec4(fL, fR, fT, fB));
    }

    LOOP_MAP(this->_output)
    {
        glm::dvec4 flux = this->f.at(x, y);
        double f_out_L = flux.x;
        double f_out_R = flux.y;
        double f_out_T = flux.z;
        double f_out_B = flux.w;
        // f_{out} &= f_{t + \Delta t}^L(x,y) + f_{t + \Delta t}^R(x,y)
        //            + f_{t + \Delta t}^T(x,y) + f_{t + \Delta t}^B(x,y)
        double f_out = f_out_L + f_out_R + f_out_T + f_out_B;

        double f_in_R = this->f.at(x - 1, y).y;
        double f_in_L = this->f.at(x + 1, y).x;
        double f_in_B = this->f.at(x, y - 1).w;
        double f_in_T = this->f.at(x, y + 1).z;
        // f_{in} &= f_{t + \Delta t}^R(x-1,y) + f_{t + \Delta t}^L(x+1,y)
        //            + f_{t + \Delta t}^B(x,y-1) + f_{t + \Delta t}^T(x,y+1)
        double f_in = f_in_L + f_in_R + f_in_T + f_in_B;

        // \begin{aligned}
        // \Delta V(x,y) &= \Delta t \cdot \left(\sum f_{in}
        //                                       - \sum f_{out}\right) \\
        //     &= \Delta t \cdot \left(f_t^R(x-1,y) + f_t^T(x, y-1)\right. \\
        //     & \quad\quad\quad\;\;\left.+ f_t^L(x+1, y)
        //                                + f_t^B(x, y+1)\right. \\
        //     & \quad\quad\quad\;\;\left.- \sum f_{t + \Delta t}^i(x,y)\right),
        //     \quad i =L,R,T,B
        // \end{aligned}
        // (Mei et al., 2007, p. 50)
        double dV = this->dT * (f_in - f_out);

        // d_2(x,y) = d_1(x,y) + \frac{\Delta V(x,y)}{l_Xl_Y}
        // (Mei et al., 2007, p. 50)
        this->w_2.set(x, y, std::max(0.00, this->w_1.at(x, y) + dV / (this->dX * this->dY)));

        // \Delta W_X = \frac{f^R(x-1,y) - f^L(x,y) + f^R(x,y) - f^L(x+1,y)}{2}
        // (Mei et al., 2007, p. 50)
        double dW_x = (f_in_R - f_out_L + f_out_R - f_in_L) / 2.00;
        double dW_y = (f_in_B - f_out_T + f_out_B - f_in_T) / 2.00;

        // \bar{d} &= \frac{d_1(x,y) + d_2(x,y)}{2}
        // (Mei et al., 2007, p. 50)
        double d_bar = (this->w_1.at(x, y) + this->w_2.at(x, y)) / 2.00;

        double uV = 0.00;
        double vV = 0.00;

        // l_Y \cdot \bar{d} \cdot u = \Delta W_X
        // (Mei et al., 2007, p. 50)
        if (dW_x != 0.00)
            uV = dW_x / (this->dY * d_bar);

        if (dW_y != 0.00)
            vV = dW_y / (this->dX * d_bar);

        this->v.set(x, y, glm::dvec4(uV, vV, 0.00, 0.00));
    }
}

void ConverterErosionNode::erosionAndDeposition3()
{
    LOOP_MAP(this->_output)
    {
        if (this->w_2.at(x,y) == 0.00)
            continue;
        // \vec{n}^l = \left<b_t(x+1,y) - b_t(x-1,y),
        //                   b_t(x,y-1) - b_t(x,y+1),
        //                   2\right>
        glm::dvec3 l_n(this->_output.at(x + 1, y) - this->_output.at(x - 1, y),
                       this->_output.at(x, y - 1) - this->_output.at(x, y + 1),
                       2.00);

        // \vec{n}^l = \text{norm}(\vec{n}^l) 
        l_n = glm::normalize(l_n);

        // \begin{aligned}
        // \vec{n} &= \left<0, 0, 1\right>
        // \alpha &= \sin\left(\max\left(0.01, \cos^{-1}\left(\frac{\vec{n}^l
        //        \cdot \vec{n}}{||\vec{n}^l||\;||\vec{n}||}\right\right)\right)
        // \end{aligned}
        double alpha = sin(std::max(0.01, acos(glm::dot(l_n, this->n))));
                                // / (sqrt(glm::dot(l_n, l_n))
                                //   * sqrt(glm::dot(this->n, this->n))))));

        glm::dvec2 vel(this->v.at(x, y).x, this->v.at(x, y).y);

        // C(x,y) = K_c \cdot \sin(\alpha(x,y)) \cdot |\vec{v}(x,y)|
        // (Mei et al., 2007, p. 51)
        double C = this->K_c * alpha * sqrt(glm::dot(vel, vel));

        // \Delta s
        double delta = 0.00;

        // Mu is greater, water can carry more sediment
        if (C > this->s.at(x, y))
        {
            delta = this->K_s * (C - this->s.at(x,y));
            this->_erosion.set(x, y, this->_erosion.at(x, y) + delta);
        }

        // \begin{aligned}
        // b_{t + \Delta t} &= b_t - K_s(C - s_t) \\
        // s_1 &= s_t + K_s(C - s_t) \\
        // d_{t + \Delta t} &= d_t + K_s(C - s_t)
        // \end{aligned}
        // (Mei et al., 2007, p. 51)
        
        // Mu is lesser, water needs to deposit sediment
        else if (C < this->s.at(x, y))
        {
            delta = this->K_d * (C - this->s.at(x,y));
            this->_sediment.set(x, y, this->_sediment.at(x, y) + delta);
        }

        // \begin{aligned}
        // b_{t + \Delta t} &= b_t - K_d(C - s_t) \\
        // s_1 &= s_t + K_d(C - s_t) \\
        // d_{t + \Delta t} &= d_t + K_d(C - s_t)
        // \end{aligned}
        // (Mei et al., 2007, p. 51)
        double off = 0.00;
        if (this->_output.at(x, y) - delta < 0.00)
            off = -(this->_output.at(x, y) - delta);

        if (this->s.at(x, y) - delta < 0.00)
            off = -(this->s.at(x, y) - delta);

        this->_output.set(x, y, this->_output.at(x, y) - delta + off);
        this->s_1.set(x, y, this->s.at(x, y) + delta - off);
        // this->w_2.set(x, y, this->w_2.at(x, y) + delta);
    }
}

void ConverterErosionNode::sedimentTransportation4()
{
    LOOP_MAP(this->_output)
    {
        // \bar{x} = x - v \cdot \Delta t
        double x_bar = (double)x - this->v.at(x, y).x * this->dT;
        double y_bar = (double)y - this->v.at(x, y).y * this->dT;

        // \bar{x}^p = \bar{x} \lfloor \bar{x} \rfloor
        double x_p = x_bar - floor(x_bar);
        double y_p = y_bar - floor(y_bar);

        // \begin{aligned}
        // x_0 &= \text{clamp}\left(\lfloor \bar{x} \rfloor, 0, \text{width}
        //                                                      - 1\right) \\
        // x_1 &= \text{clamp}\left(x_0 + 1, 0, \text{width} - 1\right)
        // \end{aligned}
        int x_0 = clamp((int)floor(x_bar), 0, this->_output.width - 1);
        int y_0 = clamp((int)floor(y_bar), 0, this->_output.height - 1);
        int x_1 = clamp(x_0 + 1, 0, this->_output.width - 1);
        int y_1 = clamp(y_0 + 1, 0, this->_output.height - 1);

        // \text{mix}(x,y,a) = x \cdot (1 - a) + y \cdot a

        // We implicitly interpolate from the 4 closest values, hence the mixes.
        // If we land on an exact grid, interpolation will simply become that
        // specific value
        // s_{t + \Delta t}(x,y) = s_1(x - u \cdot \Delta t,
        //                             y - v \cdot \Delta t)
        // (Mei et al., 2007, p. 51)
        this->s.set(x, y, mix(mix(this->s_1.at(x_0, y_0),
                                  this->s_1.at(x_1, y_0),
                                  x_p),
                             mix(this->s_1.at(x_0, y_1),
                                 this->s_1.at(x_1, y_1),
                                 x_p),
                             y_p));
    }
}

void ConverterErosionNode::evaporation5()
{
    LOOP_MAP(this->_output)
    {
        // d_{t + \Delta t}(x,y) = d_2(x,y) \cdot (1 - K_e \cdot \Delta t)
        // (Mei et al., 2007, p. 51)
        this->w.set(x, y, this->w_2.at(x, y) * (1.00 - this->K_e * this->dT));
    }
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
    int tw = this->_output.width;
    int th = this->_output.height;
    this->w = IntensityMap(tw, th, 0.00);
    this->w_1 = IntensityMap(tw, th, 0.00);
    this->w_2 = IntensityMap(tw, th, 0.00);
    this->s = IntensityMap(tw, th, 0.00);
    this->s_1 = IntensityMap(tw, th, 0.00);
    this->f = VectorMap(tw, th, glm::dvec4(0.00, 0.00, 0.00, 0.00));
    this->v = VectorMap(tw, th, glm::dvec4(0.00, 0.00, 0.00, 0.00));
    this->r = IntensityMap(tw, th, 0.00);

    this->_sediment = IntensityMap(tw, th, 0.00);
    this->_erosion = IntensityMap(tw, th, 0.00);

    this->p = this->dT * this->A * this->g / this->L;

    for (int y = th / 4; y < th; y += th / 2)
        for (int x = tw / 4; x < tw; x += tw / 2)
            this->r.set(x, y, 0.50);

    for (int i = 0; i < this->I; i++)
    {
        this->waterIncrement1();
        this->flowSimulation2();
        this->erosionAndDeposition3();
        this->sedimentTransportation4();
        this->evaporation5();
        this->smooth();
        this->r = IntensityMap(tw, tw, 0.00);
    }

    this->_output = this->_output.transform([](double a, double b) -> double {
        return a + b;
    }, &this->s);

    emit this->dataUpdated(0);
    emit this->dataUpdated(1);
    emit this->dataUpdated(2);
}