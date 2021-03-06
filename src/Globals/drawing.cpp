#include "drawing.h"

#include <QAbstractItemView>
#include <QBrush>
#include <QColorDialog>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QEvent>
#include <QGridLayout>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMouseEvent>
#include <QPen>
#include <QPixmap>
#include <QPoint>
#include <QRegExp>
#include <QWheelEvent>

#include "ui_TextureDrawingCancelDialogue.h"

// Singleton reference values
bool DrawingDialogue::_instance = false;
DrawingDialogue *DrawingDialogue::_single = nullptr;

/**
 * DrawingDialogue
 * 
 * Creates the drawing interface singleton. Gets a list of stencils and places
 * them into the stencil selection list. Gets a list of textures already loaded
 * and places them into a visual texture list.
 * 
 * TODO: include overlay of heightmap on OpenGL terrain.
 */
DrawingDialogue::DrawingDialogue()
{
    Q_CHECK_PTR(STENCILS);
    // Set the initial stencil value
    this->_active_stencil = STENCILS->stencil(0);

    Q_CHECK_PTR(this->_active_stencil);

    // Setup ui and attach listeners
    this->_ui.setupUi(&this->_dialogue);

    // Update brush size
    QObject::connect(this->_ui.spin_brush_size,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        Q_ASSERT(value >= 0);
        this->_brush = value;
        if (this->_active_stencil)
            this->_active_stencil->setBrush(this->_brush);
    });

    // Update opacity of brush
    QObject::connect(this->_ui.spin_opacity,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        Q_ASSERT(0.0 <= value && value <= 100.0);
        this->_opacity = value;
        if (this->_active_stencil)
            this->_active_stencil->setAlpha(this->_opacity / 100.00);
    });

    // Update flow rate of brush
    QObject::connect(this->_ui.spin_flow_rate,
                     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [this](double value)
    {
        Q_ASSERT(0.0 <= value && value <= 100.0);
        this->_flow_rate = value;
    });

    // Select painting colour
    QObject::connect(this->_ui.color_button, &QPushButton::clicked, [this]() {
        QColor color = QColorDialog::getColor(this->_color);
        if (color.isValid() && this->_active_stencil)
        {
            this->_color = color;
            this->_active_stencil->setColor(this->_color);
            this->_ui.color_button->setStyleSheet(
                "background-color: rgb("
                + QString::number(this->_color.red())
                                  + ", "
                                  + QString::number(this->_color.green())
                                                    + ", "
                                                    + QString::number(
                                                        this->_color.blue())
                                                        + ");");
        }
    });

    // Undo previous action
    QObject::connect(this->_ui.undo_button,
                     &QPushButton::clicked,
                     this,
                     &DrawingDialogue::undo);

    // Accept changes, combine into texture
    QObject::connect(this->_ui.ok_button, &QPushButton::clicked, [this]() {
        if (this->_original_texture != nullptr)
            this->_original_texture->replace(this->_combined_pixmap);

        this->_applied = true;
        this->_dialogue.accept();
    });

    // Cancel changes
    QObject::connect(this->_ui.cancel_button, &QPushButton::clicked, [this]() {
        if (!this->_applied)
        {
            QDialog dialogue;
            Ui::TextureDrawingCancelDialogue ui;
            ui.setupUi(&dialogue);
            if (dialogue.exec() == QDialog::Accepted)
            {
                this->_applied = true;
                this->_dialogue.accept();
            }
        }
        else
        {
            this->_dialogue.accept();
        }
    });

    // Update list of textures when a texture is added
    QObject::connect(TEXTURES,
                     &TextureList::textureAdded,
                     this,
                     &DrawingDialogue::newTexture);

    // Texture selection override (prevents default single click to select, must
    // be explicit double click)
    QObject::connect(this->_ui.texture_list,
                     &QAbstractItemView::pressed,
                     [this]()
    {
        this->_ui.texture_list->setCurrentIndex(this->_selected_model);
    });
    QObject::connect(this->_ui.texture_list,
                     &QAbstractItemView::clicked,
                     [this]()
    {
        this->_ui.texture_list->setCurrentIndex(this->_selected_model);
    });
    // Double click, select different texture to work on
    QObject::connect(this->_ui.texture_list,
                     &QAbstractItemView::doubleClicked,
                     [this](const QModelIndex &index)
    {
        if (!this->_applied)
        {
            QDialog dialogue;
            Ui::TextureDrawingCancelDialogue ui;
            ui.setupUi(&dialogue);
            if (dialogue.exec() == QDialog::Accepted)
            {
                this->_selected_model = index;
                this->show(this->_selected_model.row());
                this->_applied = true;
            }
        }
        else
        {
            this->_selected_model = index;
            this->show(this->_selected_model.row());
        }
    });

    // Setup graphics scene and attach listeners
    this->_scene = new QGraphicsScene();

    this->_ui.interact_window->setScene(this->_scene);

    this->_ui.interact_window->installEventFilter(this);
    this->_ui.interact_window->viewport()->installEventFilter(this);

    this->_ui.texture_list->setModel(&this->_texture_list_model);

    // Create buttons for different stencils
    int row = 0;
    for (int i = 0; i < STENCILS->count(); i++)
    {
        // Create stencil button from stencil
        QPushButton *button = new QPushButton(STENCILS->stencil(i)->icon(), "");
        button->setFixedSize(48, 48);
        button->setStyleSheet("padding: 0px; padding-top: -1px;");
        button->setCheckable(true);
        button->setIconSize(QSize(38, 38));
        button->setToolTip(STENCILS->stencil(i)->name());

        // Default stencil
        if (i == 0)
            button->setChecked(true);

        // 4 stencils per row
        if (i % 4 == 0 && i != 0)
            row++;

        // Place into grid
        this->_ui.stencil_grid->addWidget(button, row, i % 4);
        this->_stencil_buttons.push_back(button);

        // When clicked change active stencil
        QObject::connect(button, &QPushButton::clicked, [this, button]() {
            this->_stencil_buttons[this->_active_stencil_button]->
                setChecked(false);

            for (int i = 0; i < (int)this->_stencil_buttons.size(); i++)
            {
                if (this->_stencil_buttons[i] == button)
                {
                    this->_active_stencil_button = i;
                    this->_stencil_buttons[this->_active_stencil_button]->
                        setChecked(true);

                    this->_active_stencil = STENCILS->stencil(i);

                    Q_CHECK_PTR(this->_active_stencil);

                    this->_active_stencil->setBrush(this->_brush);
                    this->_active_stencil->setAlpha(this->_opacity / 100.00);
                    this->_active_stencil->setColor(this->_color);
                    break;
                }
            }
        });
    }

    // Create brush size/shape/location indicator
    this->_brush_indicator = this->_scene->addEllipse(
        0.00,
        0.00,
        this->_brush,
        this->_brush,
        QPen(QBrush(QColor(0, 0, 0, 255 / 2)), 1),
        QBrush(QColor(0, 0, 0, 0)));

    this->_brush_indicator->setZValue(1.0);

    Q_CHECK_PTR(TEXTURES);

    for (int i = 0; i < TEXTURES->count(); i++)
        this->newTexture(i);
}

/**
 * ~DrawingDialogue
 * 
 * Deletes the singleton, removing reference to being instantiated.
 */
DrawingDialogue::~DrawingDialogue()
{
    _instance = false;
}

/**
 * getInstance
 * 
 * Returns a pointer reference to the DrawingDialogue singleton. If the
 * singleton does not exist yet the singleton is created before being shared.
 * 
 * @returns DrawingDialogue* : Returns a pointer to the singleton.
 */
DrawingDialogue *DrawingDialogue::getInstance()
{
    if (!_instance)
    {
        _instance = true;
        _single = new DrawingDialogue();
    }
    Q_CHECK_PTR(_single);
    return _single;
}

/**
 * show
 * 
 * Shows the drawing dialogue.
 */
void DrawingDialogue::show()
{
    this->_dialogue.show();
}

// Show the dialogue with the selected texture
/**
 * Show
 * 
 * Shows the dialogue selecting a specific texture to be edited/displayed.
 * 
 * @param int index : The index of the texture to display for editing/viewing.
 */
void DrawingDialogue::show(int index)
{
    Q_CHECK_PTR(TEXTURES);
    Texture *texture = TEXTURES->at(index);
    if (texture != nullptr)
    {
        qDebug("Select texture (%d) %s", index, qPrintable(texture->name()));

        this->_original_texture = texture;

        if (this->_applied_painter != nullptr)
            this->_applied_painter->end();

        if (this->_drawing_painter != nullptr)
            this->_drawing_painter->end();

        // Create drawing surfaces
        this->_combined_pixmap = this->_original_texture->pixmap();
        this->_applied_pixmap = QPixmap(this->_combined_pixmap.width(),
                                        this->_combined_pixmap.height());

        this->_applied_pixmap.fill(QColor(255, 255, 255, 0));
        this->_drawing_pixmap = QPixmap(this->_combined_pixmap.width(),
                                        this->_combined_pixmap.height());

        this->_drawing_pixmap.fill(QColor(255, 255, 255, 0));

        this->_history.clear();

        Q_CHECK_PTR(this->_scene);

        this->_pixmap = this->_scene->addPixmap(this->_combined_pixmap);

        this->_scene->setSceneRect(-10.00,
                                   -10.00,
                                   (double)this->_combined_pixmap.width()
                                   + 10.00,
                                   (double)this->_combined_pixmap.height()
                                   + 10.00);

        this->_ui.texture_list->setCurrentIndex(
            this->_texture_list_model.index(index, 0));

        // Create drawing painters
        if (this->_applied_painter == nullptr)
            this->_applied_painter = new QPainter(&this->_applied_pixmap);
        else
            this->_applied_painter->begin(&this->_applied_pixmap);

        if (this->_drawing_painter == nullptr)
            this->_drawing_painter = new QPainter(&this->_drawing_pixmap);
        else
            this->_drawing_painter->begin(&this->_drawing_pixmap);

        // Show surfaces
        this->textureUpdated();
    }
    this->_dialogue.show();
}

/**
 * eventFilter
 * 
 * Filters events that are done to the viewport, this is used for drawing onto
 * the image canvas.
 * 
 * @param QObject* object : Must be set for the eventFilter by Qt standards.
 *                          However the parameter is not used.
 * @param QEvent* event  : The event that was done, depending on if the
 *                         event was a mouse press/release/move/wheel or key
 *                         press different actions are taken.
 */
bool DrawingDialogue::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
    // A key press was done. If the keypress matches the undo sequence (ctrl+z)
    // The previous action made is removed from the history
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *key = dynamic_cast<QKeyEvent *>(event);
        if (key->matches(QKeySequence::Undo))
            this->undo();
        return true;
    }

    Q_CHECK_PTR(this->_active_stencil);

    // The mouse wheel is changed, if the user scrolls forward we zoom into the
    // image. If the user scrolls backward we zoom out.
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

    // When the user clicks we set the previous click position to this position
    // and we draw the stencil at that location.
    else if (event->type() == QEvent::MouseButtonPress)
    {
        Q_CHECK_PTR(this->_drawing_painter);
        QMouseEvent *mouse = dynamic_cast<QMouseEvent *>(event);
        if (mouse->buttons() == Qt::LeftButton)
        {
            this->_prev = this->_ui.interact_window->mapToScene(mouse->pos());
            this->_active_stencil->draw(this->_drawing_painter, this->_prev);
            this->textureUpdated();
            this->_applied = false;
            return true;
        }
    }

    // When the user releases the button the current set of drawings onto the
    // drawing pixmap is placed onto the history stack.
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        Q_CHECK_PTR(this->_applied_painter);
        QMouseEvent *mouse = dynamic_cast<QMouseEvent *>(event);
        if (mouse->button() == Qt::LeftButton)
        {
            this->_history.push_back(this->_drawing_pixmap);
            this->_drawing_pixmap.fill(QColor(255, 255, 255, 0));
            // TODO: Replace hardcoded pixmap count history limit with memory
            //       based application level settings option
            if ((int)this->_history.size() >= 10)
            {
                this->_applied_painter->drawPixmap(0, 0, this->_history[0]);
                this->_history.erase(this->_history.begin());
            }
        }
    }

    // As the user moves the mouse, so long as the mouse left button is pressed
    // the user will draw stencils. The drawing rate of the stencils (distance
    // between two stencils) is determined by the flow rate.
    else if (event->type() == QEvent::MouseMove)
    {
        Q_CHECK_PTR(this->_drawing_painter);
        QMouseEvent *mouse = dynamic_cast<QMouseEvent *>(event);
        QPointF pos = this->_ui.interact_window->mapToScene(mouse->pos());
        if (mouse->buttons() == Qt::LeftButton)
        {
            QLineF line(this->_prev, pos);
            // Only draw new stencils if the line is longer than the flow rate
            // value
            if (line.length() >= 102.00 - this->_flow_rate)
            {
                if (this->_original_texture != nullptr)
                {
                    this->_active_stencil->draw(this->_drawing_painter, pos);
                    this->textureUpdated();
                }
                this->_prev = pos;
                return true;
            }
        }

        Q_CHECK_PTR(this->_brush_indicator);

        // Update brush location/size
        this->_brush_indicator->setRect(
            pos.x() - (this->_brush / 2.00) - 1.00,
            pos.y() - (this->_brush / 2.00) - 1.00,
            this->_brush,
            this->_brush);
    }
    return false;
}

/**
 * undo @slot
 * 
 * When called the top most action (latest drawing action) is removed from the
 * history stack. No redo is supported yet.
 * 
 * TODO: Implement redo functionality.
 */
void DrawingDialogue::undo()
{
    if ((int)this->_history.size() <= 0)
        return;
    this->_history.erase(this->_history.end());
    this->textureUpdated();
}

/**
 * textureUpdated @slot
 * 
 * Function is called when the user draws on the surface, this merges the
 * texture layer, the current drawing layer, and the history stack into a single
 * pixmap that displays the resulting drawn texture.
 */
void DrawingDialogue::textureUpdated()
{
    Q_CHECK_PTR(this->_original_texture);
    this->_combined_pixmap = this->_original_texture->pixmap();
    QPainter *painter = new QPainter(&this->_combined_pixmap);
    painter->drawPixmap(0, 0, this->_applied_pixmap);
    for (int i = 0; i < (int)this->_history.size(); i++)
        painter->drawPixmap(0, 0, this->_history[i]);

    painter->drawPixmap(0, 0, this->_drawing_pixmap);
    this->_pixmap->setPixmap(this->_combined_pixmap);

    delete painter;
}

// New texture update texture selection list
/**
 * newTexture @slot
 * 
 * When the TextureList (TEXTURES) has created a new texture we listen for a
 * signal and update the texture list displayed in the ui.
 * 
 * @param int index : The new texture index in the TextureList.
 */
void DrawingDialogue::newTexture(int index)
{
    this->_texture_list_model.appendRow(
        new QStandardItem(
            TEXTURES->at(index)->name().replace(
                QRegExp("\\.[0-9a-zA-Z]+$"), "")));
                
    if (index == 0)
    {
        this->_selected_model = this->_ui.texture_list->indexAt(QPoint(0, 0));
        this->_ui.texture_list->setCurrentIndex(this->_selected_model);
    }
}