#include "inputdrawingnode.h"

#include "../Datatypes/pixmap.h"

#include <QRect>
#include <QDebug>
#include <QColor>
#include <QBrush>
#include <QMouseEvent>
#include <QPen>

InputDrawingNode::InputDrawingNode()
{
    this->_pixmap = new QPixmap(128, 128);
    this->_painter = new QPainter(this->_pixmap);

    this->_painter->setBrush(QBrush(QColor(255, 255, 255, 255)));
    this->_painter->drawRect(QRect(0, 0, 128, 128));

    this->_widget = new QLabel();
    this->_shared = new QWidget();
    this->_widget->setPixmap(this->_pixmap->scaled(100, 100, Qt::KeepAspectRatio));
    this->_intensity_map = IntensityMap(this->_pixmap->scaled(128, 128, Qt::KeepAspectRatio), IntensityMap::AVERAGE);

    this->_ui.setupUi(this->_shared);
    this->_ui.DrawingSurface->setPixmap(this->_pixmap->scaled(128, 128, Qt::KeepAspectRatio));

    this->_ui.DrawingSurface->installEventFilter(this);
}
InputDrawingNode::~InputDrawingNode() {}

QString InputDrawingNode::caption() const
{
    return "Input Drawing Texture";
}

QString InputDrawingNode::name() const
{
    return "InputDrawingNode";
}
void InputDrawingNode::name(QString name)
{
    (void)name;
}

QWidget *InputDrawingNode::embeddedWidget()
{
    return this->_widget;
}
QWidget *InputDrawingNode::sharedWidget()
{
    return this->_shared;
}

QString InputDrawingNode::modelName()
{
    return "Input Drawing Texture Node";
}

unsigned int InputDrawingNode::nPorts(QtNodes::PortType port_type) const
{
    return port_type == QtNodes::PortType::Out ? 1 : 0;
}

// Start with simple mask type
QtNodes::NodeDataType InputDrawingNode::dataType(QtNodes::PortType port_type, QtNodes::PortIndex port_index) const
{
    (void)port_type;
    (void)port_index;
    return IntensityMapData().type();
}

std::shared_ptr<QtNodes::NodeData> InputDrawingNode::outData(QtNodes::PortIndex port)
{
    (void)port;
    return std::make_shared<IntensityMapData>(this->_intensity_map);
}

QJsonObject InputDrawingNode::save() const
{
    QJsonObject data;
    data["name"] = this->name();
    return data;
}

void InputDrawingNode::restore(QJsonObject const &data)
{
    (void)data;
}

void InputDrawingNode::setInData(std::shared_ptr<QtNodes::NodeData> node_data, QtNodes::PortIndex port)
{
    (void)node_data;
    (void)port;
}
bool InputDrawingNode::eventFilter(QObject *object, QEvent *event)
{
    if (object == this->_ui.DrawingSurface)
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

        if (!this->_prev_first)
        {
            this->_prev_first = true;
            this->_prev = pos;
        }

        this->_painter->setBrush(QBrush(QColor(0, 0, 0, 0)));
        QPen pen(QColor(0, 0, 0, 255));
        pen.setWidth(3);
        this->_painter->setPen(pen);
        this->_painter->drawLine(this->_prev, pos);

        this->_ui.DrawingSurface->setPixmap(this->_pixmap->scaled(128, 128, Qt::KeepAspectRatio));
        this->_widget->setPixmap(this->_pixmap->scaled(100, 100, Qt::KeepAspectRatio));
        this->_intensity_map = IntensityMap(this->_pixmap->scaled(128, 128, Qt::KeepAspectRatio), IntensityMap::AVERAGE);

        this->_prev = pos;

        emit this->dataUpdated(0);

        return true;
    }
    return false;
}