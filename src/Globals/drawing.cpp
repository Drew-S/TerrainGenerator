#include "drawing.h"

#include <QDebug>
#include <QPixmap>
#include <QDoubleSpinBox>
#include <QEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QAbstractItemView>
#include <QGridLayout>
#include <QColorDialog>

bool DrawingDialogue::_instance = false;
DrawingDialogue *DrawingDialogue::_single = nullptr;

// Create the dialogue
// TODO: include overlay of heightmap on OpenGL terrain
// TODO: implement history/save/apply system
DrawingDialogue::DrawingDialogue()
{
    // Set the initial stencil value
    this->_active_stencil = STENCILS->stencil(0);

    // Setup ui and attach listeners
    this->_ui.setupUi(&this->_dialogue);

    QObject::connect(this->_ui.spin_brush_size, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_brush = value;
        if (this->_active_stencil)
            this->_active_stencil->setBrush(this->_brush);
    });

    QObject::connect(this->_ui.spin_opacity, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_opacity = value;
        if (this->_active_stencil)
            this->_active_stencil->setAlpha(this->_opacity / 100.00);
    });

    QObject::connect(this->_ui.spin_flow_rate, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        this->_flow_rate = value;
    });

    QObject::connect(this->_ui.color_button, &QPushButton::clicked, [this]() {
        QColor color = QColorDialog::getColor(this->_color);
        if (color.isValid() && this->_active_stencil)
        {
            this->_color = color;
            this->_active_stencil->setColor(this->_color);
            this->_ui.color_button->setStyleSheet("background-color: rgb(" + QString::number(this->_color.red()) + ", " + QString::number(this->_color.green()) + ", " + QString::number(this->_color.blue()) + ");");
        }
    });

    QObject::connect(this->_ui.ok_button, &QPushButton::clicked, [this]() {
        if (this->_original_texture != nullptr)
            this->_original_texture->apply();
        this->_dialogue.accept();
    });

    QObject::connect(TEXTURES, &TextureList::textureAdded, this, &DrawingDialogue::newTexture);

    QObject::connect(this->_ui.texture_list, &QAbstractItemView::pressed, [this]() {
        this->_ui.texture_list->setCurrentIndex(this->_selected_model);
    });
    QObject::connect(this->_ui.texture_list, &QAbstractItemView::clicked, [this]() {
        this->_ui.texture_list->setCurrentIndex(this->_selected_model);
    });
    QObject::connect(this->_ui.texture_list, &QAbstractItemView::doubleClicked, [this](const QModelIndex &index) {
        this->_selected_model = index;
        this->show(this->_selected_model.row());
    });

    // Setup graphics scene and attach listeners
    this->_scene = new QGraphicsScene();

    this->_ui.interact_window->setScene(this->_scene);

    this->_ui.interact_window->installEventFilter(this);
    this->_ui.interact_window->viewport()->installEventFilter(this);

    this->_ui.texture_list->setModel(&this->_texture_list_model);

    int row = 0;
    for (int i = 0; i < STENCILS->count(); i++)
    {
        QPushButton *button = new QPushButton(STENCILS->stencil(i)->icon(), "");
        button->setFixedSize(48, 48);
        button->setStyleSheet("padding: 0px; padding-top: -1px;");
        button->setCheckable(true);
        button->setIconSize(QSize(38, 38));
        button->setToolTip(STENCILS->stencil(i)->name());
        if (i == 0)
            button->setChecked(true);
        if (i % 4 == 0 && i != 0)
            row++;
        this->_ui.stencil_grid->addWidget(button, row, i % 4);
        this->_stencil_buttons.push_back(button);
        QObject::connect(button, &QPushButton::clicked, [this, button]() {
            this->_stencil_buttons[this->_active_stencil_button]->setChecked(false);
            for (int i = 0; i < (int)this->_stencil_buttons.size(); i++)
            {
                if (this->_stencil_buttons[i] == button)
                {
                    this->_active_stencil_button = i;
                    this->_stencil_buttons[this->_active_stencil_button]->setChecked(true);
                    this->_active_stencil = STENCILS->stencil(i);
                    this->_active_stencil->setBrush(this->_brush);
                    this->_active_stencil->setAlpha(this->_opacity / 100.00);
                    this->_active_stencil->setColor(this->_color);
                    break;
                }
            }
        });
    }

    for (int i = 0; i < TEXTURES->count(); i++)
        this->newTexture(i);
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
        qDebug("Select texture (%d) %s", index, qPrintable(texture->name()));
        if (this->_original_texture != nullptr)
            QObject::disconnect(this->_original_texture, &Texture::updated, this, &DrawingDialogue::textureUpdated);

        this->_original_texture = texture;

        QObject::connect(this->_original_texture, &Texture::updated, this, &DrawingDialogue::textureUpdated);

        QPixmap pixmap = this->_original_texture->pixmap();

        this->_pixmap = this->_scene->addPixmap(pixmap);
        this->_scene->setSceneRect(-10.00, -10.00, (double)pixmap.width() + 10.00, (double)pixmap.height() + 10.00);
        this->_ui.texture_list->setCurrentIndex(this->_texture_list_model.index(index, 0));
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
            this->_original_texture->draw(this->_active_stencil, this->_prev);
            this->textureUpdated();
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
                if (this->_original_texture != nullptr)
                {
                    this->_original_texture->draw(this->_active_stencil, pos);
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
    this->_pixmap->setPixmap(this->_original_texture->pixmap());
}

void DrawingDialogue::newTexture(int index)
{
    this->_texture_list_model.appendRow(new QStandardItem(TEXTURES->at(index)->name()));
    if (index == 0)
    {
        this->_selected_model = this->_ui.texture_list->indexAt(QPoint(0, 0));
        this->_ui.texture_list->setCurrentIndex(this->_selected_model);
    }
}