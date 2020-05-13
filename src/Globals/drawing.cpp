#include "drawing.h"

#include <QDebug>
#include <QPixmap>
#include <QColor>
#include <QDoubleSpinBox>
#include <QEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QPushButton>

bool DrawingDialogue::_instance = false;
DrawingDialogue *DrawingDialogue::_single = nullptr;

// Create the dialogue
DrawingDialogue::DrawingDialogue()
{
    // Set the initial stencil value
    this->_active_stencil = STENCILS->stencil(0);

    // Setup ui and attach listeners
    this->_ui.setupUi(&this->_dialogue);

    QObject::connect(this->_ui.spin_brush_size, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        if (this->_active_stencil)
            this->_active_stencil->setBrush(value);
    });

    QObject::connect(this->_ui.spin_opacity, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        if (this->_active_stencil)
            this->_active_stencil->setAlpha(value / 100.00);
    });

    QObject::connect(this->_ui.spin_flow_rate, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_flow_rate = value;
    });

    QObject::connect(this->_ui.ok_button, &QPushButton::clicked, [this]() {
        if (this->_active_texture != nullptr)
            this->_active_texture->apply();
        this->_dialogue.accept();
    });

    // Setup graphics scene and attach listeners
    this->_scene = new QGraphicsScene();

    this->_ui.interact_window->setScene(this->_scene);

    this->_ui.interact_window->installEventFilter(this);
    this->_ui.interact_window->viewport()->installEventFilter(this);
}

// Remove singleton
DrawingDialogue::~DrawingDialogue()
{
    _instance = false;
}

// Get/create singleton reference
DrawingDialogue *DrawingDialogue::getInstance()
{
    if (!_instance)
    {
        _instance = true;
        _single = new DrawingDialogue();
    }
    return _single;
}

// Show the dialogue
void DrawingDialogue::show()
{
    this->_dialogue.exec();
}

// Show the dialogue with the selected texture
void DrawingDialogue::show(int index)
{
    Texture *texture = TEXTURES->at(index);
    if (texture != nullptr)
    {
        if (this->_active_texture != nullptr)
            QObject::disconnect(this->_active_texture, &Texture::updated, this, &DrawingDialogue::textureUpdated);

        this->_active_texture = texture;

        QObject::connect(this->_active_texture, &Texture::updated, this, &DrawingDialogue::textureUpdated);

        QPixmap pixmap = this->_active_texture->pixmap();

        this->_pixmap = this->_scene->addPixmap(pixmap);
        this->_scene->setSceneRect(-10.00, -10.00, (double)pixmap.width() + 10.00, (double)pixmap.height() + 10.00);
    }
    this->_dialogue.exec();
}

// Handle drawing and zooming of the texture
bool DrawingDialogue::eventFilter(QObject *object, QEvent *event)
{
    (void)object;

    // Zoom into/out the texture
    if (event->type() == QEvent::Wheel)
    {
        QWheelEvent *wheel = dynamic_cast<QWheelEvent *>(event);
        int change = wheel->angleDelta().y();
        if (change > 0)
            this->_ui.interact_window->scale(1.1, 1.1);
        else
            this->_ui.interact_window->scale(0.9, 0.9);
        return true;
    }

    // On a click, place a stencil
    else if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouse = dynamic_cast<QMouseEvent *>(event);
        if (mouse->buttons() == Qt::LeftButton)
        {
            this->_prev = this->_ui.interact_window->mapToScene(mouse->pos());
            return true;
        }
    }

    // On drag place stencils
    else if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *mouse = dynamic_cast<QMouseEvent *>(event);
        if (mouse->buttons() == Qt::LeftButton)
        {
            QPointF pos = this->_ui.interact_window->mapToScene(mouse->pos());
            QLineF line(this->_prev, pos);
            // Only draw new stencils if the line is longer than the flow rate value
            if (line.length() >= 101.00 - this->_flow_rate)
            {
                if (this->_active_texture != nullptr)
                {
                    this->_active_texture->draw(this->_active_stencil, pos);
                    this->textureUpdated();
                }
                this->_prev = pos;
                return true;
            }
        }
    }
    return false;
}

// When the texture is updated (drawn on) update the visual pixmap
void DrawingDialogue::textureUpdated()
{
    this->_pixmap->setPixmap(this->_active_texture->pixmap());
}