#include "mainwindow.h"

#include <math.h>
#include <string>

#include <QBuffer>
#include <QByteArray>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QFileDevice>
#include <QFileDialog>
#include <QFileInfo>
#include <QHelpContentWidget>
#include <QHelpEngine>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QPushButton>
#include <QRegExp>
#include <QSplitter>
#include <QTabWidget>
#include <QTextBrowser>

#include <json.hpp>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include <quazip/quazipnewinfo.h>

#include <nodes/ConnectionStyle>
#include <nodes/FlowViewStyle>
#include <nodes/NodeStyle>

#include "Globals/drawing.h"
#include "Globals/settings.h"
#include "Globals/stencillist.h"
#include "Globals/texturelist.h"

using json = nlohmann::json;

// Zips internal datafile name
#define SAVE_DATA_FILE_NAME "data.json"

// Save filename extension
#define EXT ".tgdf"

// Save filename extension (regex)
#define EXT_REG "\\.tgdf$"

// https://doc.qt.io/qt-5/qfiledevice.html#Permission-enum
// linux 666 permissions, Read/Write all (no execute)
#define RW_ALL (QFileDevice::Permission)0x6666

// Assertion macro for ensuring a number is between two values
#define Q_BETWEEN(low, v, hi) Q_ASSERT(low <= v && v <= hi)

/**
 * setup
 * 
 * Used to setup the mainwindows UI elements. Calls global singletons to ensure
 * they are setup. Attaches UI to the widget. Attaches listeners to the UI.
 * 
 * @param Ui::MainWindow* ui : The ui (QWidget) created from Qt Designer that
 *                                is what is viewed.
 * 
 * TODO: Clean up, setup and lambdas are a bit larger -> add sub headers
 */
void MainWindow::setup(Ui::MainWindow *ui)
{
    // Ensure global singletons have loaded
    Q_CHECK_PTR(SETTINGS);
    Q_CHECK_PTR(TEXTURES);
    Q_CHECK_PTR(DRAWING);
    Q_CHECK_PTR(STENCILS);
    SETTINGS;
    TEXTURES;
    DRAWING;
    STENCILS;

    qDebug("Setting up main window and save dialogue ui, attaching listeners");

    // Attach/create UI elements
    this->_main_ui = ui;
    this->_editor = new Nodeeditor(ui->NodeEditorLayout,
                                   ui->NodePropertiesContainerWidget);
    this->_open_gl = this->_main_ui->OpenGLWidget;

    this->_render = new QDialog();
    this->_render_ui.setupUi(this->_render);

    this->_render_ui.directory->setText(this->_render_directory);

    this->_render_progress = new QDialog();
    this->_render_progress_ui.setupUi(this->_render_progress);

    QObject::connect(this->_render_ui.directory_select,
                     &QPushButton::clicked,
                     [this]() {
                         QString dir = QFileDialog::getExistingDirectory(
                             this, tr("Select Output Directory"),
                             this->_render_directory
                         );

                         if (dir != "")
                             this->_render_directory = dir;

                         this->_render_ui.directory->setText(
                             this->_render_directory);
                     });

    QObject::connect(this->_render_ui.cancel,
                     &QPushButton::clicked,
                     this->_render,
                     &QWidget::hide);

    QObject::connect(this->_render_ui.ok,
                     &QPushButton::clicked,
                     [this]() {
                         Q_CHECK_PTR(SETTINGS);
                         this->_render->hide();
                         this->_render_progress->show();
                         this->_render_progress_ui.progress->setValue(0);
                         SETTINGS->setRunRender(true);
                         SETTINGS->setRenderMode(true);
                     });

    // Listen for editor to signal the dataflow diagram has updated the output
    QObject::connect(this->_editor,
                     &Nodeeditor::outputUpdated,
                     [this](QImage normal_map,
                            QImage height_map,
                            QImage albedo_map) {
                         Q_CHECK_PTR(SETTINGS);
                         if (SETTINGS->runRender())
                         {
                             normal_map.save(
                                 QDir::cleanPath(
                                     this->_render_directory + QString("/normalmap.png")));
                             this->_render_progress_ui.progress->setValue(50);
                             height_map.save(
                                 QDir::cleanPath(
                                     this->_render_directory + QString("/heightmap.png")));
                             this->_render_progress_ui.progress->setValue(100);
                             this->_render_progress->hide();
                         }
                         else
                         {
                             this->_open_gl->nodeeditorOutputUpdated(
                                 normal_map,
                                 height_map,
                                 albedo_map);
                         }
                     });

    // Connect file actions to load and save files
    QObject::connect(this->_main_ui->actionSave_As,
                     &QAction::triggered,
                     this,
                     &MainWindow::saveAs);
    QObject::connect(this->_main_ui->actionLoad,
                     &QAction::triggered,
                     this,
                     &MainWindow::load);

    // TODO: Move project settings to its own management class
    // Settings actions
    QObject::connect(this->_main_ui->combo_preview,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     [=](int index)
    {
        Q_CHECK_PTR(SETTINGS);
        Q_BETWEEN(0, index, 6);
        SETTINGS->setPreviewResolution((int)pow(2, index + 7));
    });
    QObject::connect(this->_main_ui->combo_render,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     [=](int index)
    {
        Q_CHECK_PTR(SETTINGS);
        Q_BETWEEN(0, index, 6);
        SETTINGS->setRenderResolution((int)pow(2, index + 7));
    });
    QObject::connect(this->_main_ui->use_render,
                     &QCheckBox::stateChanged,
                     [=](int state)
    {
        Q_CHECK_PTR(SETTINGS);
        SETTINGS->setRenderMode(state == 2);
    });
    QObject::connect(this->_main_ui->draw_lines,
                     &QCheckBox::stateChanged,
                     [this](int state)
    {
        Q_CHECK_PTR(this->_open_gl);
        this->_open_gl->setTerrainDrawLines(state == 2);
    });
    QObject::connect(this->_main_ui->terrain_color,
                     &QPushButton::clicked,
                     [this]()
    {
        Q_CHECK_PTR(this->_open_gl);
        QColor color = QColorDialog::getColor(this->_open_gl->terrainColor());
        if (color.isValid())
        {
            this->_open_gl->setTerrainColor(color);
            this->_main_ui->terrain_color->setStyleSheet(
                QString::asprintf("background-color: rgba(%d,%d,%d,255);",
                color.red(),
                color.green(),
                color.blue()));
        }
    });
    QObject::connect(this->_main_ui->terrain_line_color,
                     &QPushButton::clicked,
                     [this]()
    {
        Q_CHECK_PTR(this->_open_gl);
        QColor color =
            QColorDialog::getColor(this->_open_gl->terrainLineColor());

        if (color.isValid())
        {
            this->_open_gl->setTerrainLineColor(color);
            this->_main_ui->terrain_line_color->setStyleSheet(
                QString::asprintf("background-color: rgba(%d,%d,%d,255);",
                color.red(),
                color.green(),
                color.blue()));
        }
    });
    QObject::connect(this->_main_ui->combo_mesh,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     [this](int index)
    {
        Q_CHECK_PTR(this->_open_gl);
        Q_BETWEEN(0, index, 4);
        SETTINGS->setMeshResolution((int)pow(2, index + 4));
        this->_open_gl->setTerrainMeshResolution((int)pow(2, index + 4));
    });
    QObject::connect(this->_main_ui->sky_color,
                     &QPushButton::clicked,
                     [this]()
    {
        Q_CHECK_PTR(this->_open_gl);
        QColor color = QColorDialog::getColor(this->_open_gl->skyColor());
        if (color.isValid())
        {
            this->_open_gl->setSkyColor(color);
            this->_main_ui->sky_color->setStyleSheet(
                QString::asprintf("background-color: rgba(%d,%d,%d,255);",
                color.red(),
                color.green(),
                color.blue()));
        }
    });
    QObject::connect(this->_main_ui->light_color,
                     &QPushButton::clicked,
                     [this]()                 
    {
        Q_CHECK_PTR(this->_open_gl);
        QColor color = QColorDialog::getColor(this->_open_gl->lightColor());
        if (color.isValid())
        {
            this->_open_gl->setLightColor(color);
            this->_main_ui->light_color->setStyleSheet(
                QString::asprintf("background-color: rgba(%d,%d,%d,255);",
                color.red(),
                color.green(),
                color.blue()));
        }
    });
    QObject::connect(this->_main_ui->render,
                     &QPushButton::clicked,
                     this->_render,
                     &QWidget::show);

    // Fix Nodeeditor and OpenGL widget splitter size
    this->_main_ui->splitter_top_bottom->setSizes({300, 150});

    // Create a save as dialog and setup it ui
    this->_save_as_dialogue = new QDialog(this);

    this->_save_ui = new Ui::SaveAsDialogue();
    this->_save_ui->setupUi(this->_save_as_dialogue);

    // Set output directory default label
    this->_save_ui->directory_label->setText(QDir::homePath());

    // Connect directory to select output directory
    QObject::connect(this->_save_ui->directory_button,
                     &QPushButton::clicked,
                     this,
                     &MainWindow::_saveAsFile);

    // Connect line edit to define filename output (extension is automatic)
    QObject::connect(this->_save_ui->filename_line_edit,
                     &QLineEdit::textChanged,
                     this,
                     &MainWindow::_saveAsLineEdit);

    // Connect the ok button to save as to write file. Connect cancel button to
    // close the dialogue
    QObject::connect(this->_save_ui->button_box,
                     &QDialogButtonBox::accepted,
                     this,
                     &MainWindow::_saveAsAccept);
    QObject::connect(this->_save_ui->button_box,
                     &QDialogButtonBox::rejected,
                     this->_save_as_dialogue,
                     &QDialog::reject);
    QObject::connect(this->_save_ui->pack_files_check_box,
                     &QCheckBox::clicked,
                     this,
                     &MainWindow::_saveAsTogglePack);

    this->_help = new QDialog();
    this->_help_ui.setupUi(this->_help);

    QString docs = SETTINGS->getDocsDirectory().path();

    QHelpEngine *help_engine = new QHelpEngine(
        QDir::cleanPath(docs + "/help.qhc"));

    help_engine->setupData();
    
    this->_help_ui.tab->addTab(help_engine->contentWidget(), "Contents");
    // tab->addTab(help_engine->indexWidget(), "Index");

    this->_help_ui.text->setSource(
        QUrl::fromLocalFile(QDir::cleanPath(docs + "/index.md")),
        QTextDocument::MarkdownResource);

    QObject::connect(help_engine->contentWidget(),
                    &QHelpContentWidget::linkActivated,
                    [this, docs](const QUrl &link) {
                        this->_help_ui.text->setSource(
                            QUrl::fromLocalFile(QDir::cleanPath(docs + "/"
                                 + link.fileName())),
                            QTextDocument::MarkdownResource);
                    });
                    
    QObject::connect(this->_help_ui.text,
                     &QTextBrowser::anchorClicked,
                     [ this, docs ](const QUrl &link) {
                         this->_help_ui.text->setSource(
                            QUrl::fromLocalFile(QDir::cleanPath(docs + "/"
                                 + link.fileName())),
                            QTextDocument::MarkdownResource);
                     });

    // QObject::connect(help_engine->indexWidget(),
    //                  &QHelpIndexWidget::linkActivated,
    //                  text,
    //                  &QTextBrowser::setSource);

    QObject::connect(this->_main_ui->actionHelp,
                    &QAction::triggered,
                    this->_help,
                    &QWidget::show);
}

/**
 * _saveAsFile @slot
 * 
 * When called this opens a file dialogue to select the output directory for
 * saving the project to file.
 */
void MainWindow::_saveAsFile()
{
    // User selects the output directory
    QString directory = QFileDialog::getExistingDirectory(
        nullptr,
        tr("Select directory to save file to"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    qDebug("Directory to save project set to: %s", qPrintable(directory));

    Q_CHECK_PTR(this->_save_ui);

    // Update the UI label
    this->_save_ui->directory_label->setText(directory);
    this->_save_as_directory = directory;
}

/**
 * _saveAsTogglePack @slot
 * 
 * Toggles the pack external resources (linked texture files) flag. When checked
 * to true, the system will pack the files with the save file.
 * 
 * @param bool checked : Toggle value as to whether or not to pack textures.
 */
void MainWindow::_saveAsTogglePack(bool checked)
{
    Q_CHECK_PTR(SETTINGS);
    qDebug("Packing resources in save file: %s", checked ? "true" : "false");
    SETTINGS->setPackImages(checked);
}

/**
 * _saveAsLineEdit @slot
 * 
 * The user updates the filename to save the file as. This updates the save name
 * of the project file.
 * 
 * @param QString const& text : The new filename text of the save file.
 */
void MainWindow::_saveAsLineEdit(QString const &text)
{
    this->_save_as_filename = text;
}

/**
 * _saveAsAccept @slot
 * 
 * When the user selects to save the project (directory and name must be set).
 * This will save the current state of the project into the selected output
 * file. If the pack externals option is checked, external images will be packed
 * within the zip save file.
 *
 * TODO: Add indication for successfully/failing to save.
 */
void MainWindow::_saveAsAccept()
{
    // Terrain Generator Data File : tgdf
    // TODO: Come up with better extension name
    if (this->_save_as_directory != ""
        && this->_save_as_filename != ""
        && this->_save_as_filename != EXT)
    {
        // <file>.tgdf -> <file> (user having tgdf extension is the same as no
        // extension) (prevents <file>.tgdf.tgdf)
        QString filename = this->_save_as_filename;
        filename = filename.replace(QRegExp(EXT_REG), "");
        filename = this->_save_as_directory + "/" + filename + EXT;
        qInfo("Saving to file: %s", qPrintable(filename));

        // Create the save zip file
        QuaZip zip(filename);
        if (!zip.open(QuaZip::mdAdd))
        {
            qCritical("Unable to open up: '%s' for saving",
                      qPrintable(filename));

            this->_save_as_dialogue->reject();
            return;
        }
        Q_CHECK_PTR(this->_editor);
        Q_CHECK_PTR(TEXTURES);
        Q_CHECK_PTR(SETTINGS);

        // Pack image textures appropriately, (external images if the packed 
        // flag is set, or if the image is generated (drawn))
        QuaZipFile image_file(&zip);
        for (int i = 0; i < TEXTURES->count(); i++)
        {
            Texture *texture = TEXTURES->at(i);
            if (SETTINGS->packImages() || texture->generated())
            {
                QuaZipNewInfo info(texture->saveName());
                info.setPermissions(RW_ALL);
                if (!image_file.open(QIODevice::WriteOnly, info))
                {
                    qWarning("Unable to open zip file for writing");
                    continue;
                }

                if (!texture->save(&image_file))
                    qWarning("Unable to save image");

                image_file.close();
            }
        }

        // Get project data
        QJsonObject global;
        global["packed_externals"] = SETTINGS->packImages();
        global["save_version"] = "v1.1";
        global["nodes"] = this->_editor->save();

        Q_CHECK_PTR(this->_save_as_dialogue);

        // Write json project data file
        QuaZipFile file(&zip);
        QuaZipNewInfo info(SAVE_DATA_FILE_NAME);
        info.setPermissions(RW_ALL);
        if (!file.open(QIODevice::WriteOnly, info))
        {
            qCritical("Unable to open zip data file for writing");
            zip.close();
            this->_save_as_dialogue->reject();
            return;
        }
        file.write(QJsonDocument(global).toJson());
        file.close();
        zip.close();

        qDebug("Saving completed successfully");

        // Close the dialogue
        this->_save_as_dialogue->accept();
    }
    else
    {
        qInfo("Cannot save, not enough/invalid information");
        // TODO: UI validation information
        this->_save_as_dialogue->reject();
    }
}

/**
 * saveAs
 * 
 * Open the save as dialogue so the user can set the directory and filename of
 * the save file and whether or not to pack external files.
 */
void MainWindow::saveAs()
{
    Q_CHECK_PTR(this->_save_as_dialogue);
    qDebug("Open saving dialogue");
    this->_save_as_dialogue->show();
}

/**
 * load
 * 
 * Loads the project save file from the filesystem and set it as the current
 * state of the active project.
 * 
 * TODO: Implement Validator.
 * TODO: Add indication for successfully/failing to load.
 */
void MainWindow::load()
{
    Q_CHECK_PTR(TEXTURES);

    // User selects the project file to load from
    QString filename = QFileDialog::getOpenFileName(
        nullptr,
        tr("Open Project"),
        QDir::homePath(),
        tr(qPrintable(
            QString("Project files (*") + QString(EXT) + QString(")"))));

    qInfo("Attempting to load save file: %s", qPrintable(filename));

    // If no file is selected cancel
    if (filename == "")
        return;

    // Load zip file
    QuaZip zip(filename);
    if (!zip.open(QuaZip::mdUnzip))
    {
        qCritical("Unable to unzip save file");
        return;
    }

    // Read json project data file
    zip.setCurrentFile(SAVE_DATA_FILE_NAME);
    QuaZipFile file(&zip);
    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical("Unable to read zip data file");
        zip.close();
        return;
    }
    QByteArray data = file.readAll();
    file.close();

    // Loop over all files (not json data file) and load image files into the
    // system
    zip.goToFirstFile();
    do
    {
        QuaZipFile image_file(&zip);
        if (zip.getCurrentFileName() != SAVE_DATA_FILE_NAME)
        {
            if (image_file.open(QIODevice::ReadOnly))
            {
                TEXTURES->loadTexture(image_file.readAll(),
                                      zip.getCurrentFileName());
                image_file.close();
            }
        }

    } while (zip.goToNextFile());

    // Create json from data file
    QJsonDocument document = QJsonDocument::fromJson(data);

    qInfo("Save file version: %s",
          qPrintable(document["save_version"].toString()));

    Q_CHECK_PTR(SETTINGS);
    zip.close();

    // Load nodeeditor
    this->_editor->load(document["nodes"].toObject());

    qDebug("Loading save file complete");
}