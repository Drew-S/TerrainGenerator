#include "mainwindow.h"

#include <string>

#include <math.h> // pow

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include <quazip/quazipnewinfo.h>

#include <json.hpp>

using json = nlohmann::json;

#include <QObject>
#include <QImage>
#include <QDebug>
#include <QFile>
#include <QFileDevice>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QBuffer>
#include <QDir>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QRegExp>
#include <QList>
#include <QComboBox>
#include <QCheckBox>
#include <QColorDialog>

#include "Globals/settings.h"
#include "Globals/stencillist.h"
#include "Globals/texturelist.h"
#include "Globals/drawing.h"

#define SAVE_DATA_FILE_NAME "data.json"
#define EXT ".tgdf"
#define EXT_REG "\\.tgdf$"
#define RW_ALL QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadUser | QFileDevice::WriteUser | QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ReadOther | QFileDevice::WriteOther

#define Q_BETWEEN(low, v, hi) Q_ASSERT(low <= v && v <= hi)

MainWindow::MainWindow()
{
    qDebug("Using temp directory: %s", qPrintable(SETTINGS->tmpDir().path()));
}
MainWindow::~MainWindow() {}

// Setup is a function used to setup the application,
// Connect to ui elements and controls and link sub components
void MainWindow::setup(Ui::MainWindow *ui)
{
    //Ensure global singletons have loaded
    Q_CHECK_PTR(SETTINGS);
    Q_CHECK_PTR(TEXTURES);
    Q_CHECK_PTR(DRAWING);
    Q_CHECK_PTR(STENCILS);
    SETTINGS;
    TEXTURES;
    DRAWING;
    STENCILS;
    qDebug("Setting up main window and save dialogue ui, attaching listeners");
    this->_main_ui = ui;
    this->_editor = new Nodeeditor(ui->NodeEditorLayout, ui->NodePropertiesContainerWidget);
    this->_open_gl = this->_main_ui->OpenGLWidget;

    // Connect output widget compute finished to OpenGL to display results
    QObject::connect(this->_editor, &Nodeeditor::outputUpdated, this->_open_gl, &OpenGL::nodeeditorOutputUpdated);

    // Connect file actions to load and save files
    QObject::connect(this->_main_ui->actionSave_As, &QAction::triggered, this, &MainWindow::saveAs);
    QObject::connect(this->_main_ui->actionLoad, &QAction::triggered, this, &MainWindow::load);

    // Settings actions
    QObject::connect(this->_main_ui->combo_preview, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        Q_CHECK_PTR(SETTINGS);
        Q_BETWEEN(0, index, 6);
        SETTINGS->setPreviewResolution((int)pow(2, index + 7));
    });
    QObject::connect(this->_main_ui->combo_render, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        Q_CHECK_PTR(SETTINGS);
        Q_BETWEEN(0, index, 6);
        SETTINGS->setRenderResolution((int)pow(2, index + 7));
    });
    QObject::connect(this->_main_ui->use_render, &QCheckBox::stateChanged, [=](int state) {
        Q_CHECK_PTR(SETTINGS);
        SETTINGS->setRenderMode(state == 2);
    });
    QObject::connect(this->_main_ui->draw_lines, &QCheckBox::stateChanged, [this](int state) {
        Q_CHECK_PTR(this->_open_gl);
        this->_open_gl->setTerrainDrawLines(state == 2);
    });
    QObject::connect(this->_main_ui->terrain_color, &QPushButton::clicked, [this]() {
        Q_CHECK_PTR(this->_open_gl);
        QColor color = QColorDialog::getColor(this->_open_gl->terrainColor());
        if (color.isValid())
            this->_open_gl->setTerrainColor(color);
    });
    QObject::connect(this->_main_ui->terrain_line_color, &QPushButton::clicked, [this]() {
        Q_CHECK_PTR(this->_open_gl);
        QColor color = QColorDialog::getColor(this->_open_gl->terrainLineColor());
        if (color.isValid())
            this->_open_gl->setTerrainLineColor(color);
    });
    QObject::connect(this->_main_ui->combo_mesh, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        Q_CHECK_PTR(this->_open_gl);
        Q_BETWEEN(0, index, 4);
        SETTINGS->setMeshResolution((int)pow(2, index + 4));
        this->_open_gl->setTerrainMeshResolution((int)pow(2, index + 4));
    });

    // Fix Nodeeditor and OpenGL widget splitter size
    this->_main_ui->splitter_top_bottom->setSizes({300, 150});

    // Create a save as dialog and setup it ui
    this->_save_as_dialogue = new QDialog(this);

    this->_save_ui = new Ui::SaveAsDialogue();
    this->_save_ui->setupUi(this->_save_as_dialogue);

    // Set output directory default label
    this->_save_ui->directory_label->setText(QDir::homePath());

    // Connect directory to select output directory
    QObject::connect(this->_save_ui->directory_button, &QPushButton::clicked, this, &MainWindow::_saveAsFile);

    // Connect line edit to define filename output (extension is automatic)
    QObject::connect(this->_save_ui->filename_line_edit, &QLineEdit::textChanged, this, &MainWindow::_saveAsLineEdit);

    // Connect the ok button to save as to write file. Connect cancel button to close the dialogue
    QObject::connect(this->_save_ui->button_box, &QDialogButtonBox::accepted, this, &MainWindow::_saveAsAccept);
    QObject::connect(this->_save_ui->button_box, &QDialogButtonBox::rejected, this->_save_as_dialogue, &QDialog::reject);
    QObject::connect(this->_save_ui->pack_files_check_box, &QCheckBox::clicked, this, &MainWindow::_saveAsTogglePack);
}

// Open up a file dialogue for the user to select an output directory
void MainWindow::_saveAsFile()
{
    QString directory = QFileDialog::getExistingDirectory(
        nullptr,
        tr("Select directory to save file to"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    qDebug("Directory to save project set to: %s", qPrintable(directory));

    Q_CHECK_PTR(this->_save_ui);

    this->_save_ui->directory_label->setText(directory);
    this->_save_as_directory = directory;
}

// Toggle whether or not to pack images with save file
void MainWindow::_saveAsTogglePack(bool checked)
{
    qDebug("Packing resources in save file: %s", checked ? "true" : "false");
    this->_save_as_pack = checked;
}

// Update the filename when the line edit is updated
void MainWindow::_saveAsLineEdit(QString const &text)
{
    this->_save_as_filename = text;
    QString file = text;
    file.replace(QRegExp(EXT_REG), "");
    qDebug("Save file set to: %s%s", qPrintable(file), EXT);
}

// Saves the project data to the provided output file
// TODO: A bit deep, separate into sub functions
// TODO: Add indication for successfully/failing to save
void MainWindow::_saveAsAccept()
{
    // Terrain Generator Data File : tgdf
    // TODO: Come up with better extension name
    if (this->_save_as_directory != "" && this->_save_as_filename != "" && this->_save_as_filename != EXT)
    {
        // <file>.tgdf -> <file> (user having tgdf extension is the same as no extension) (prevents <file>.tgdf.tgdf)
        QString filename = this->_save_as_filename;
        filename = filename.replace(QRegExp(EXT_REG), "");
        filename = this->_save_as_directory + "/" + filename + EXT;
        qInfo("Saving to file: %s", qPrintable(filename));

        QuaZip zip(filename);
        if (!zip.open(QuaZip::mdAdd))
        {
            qCritical("Unable to open up: '%s' for saving", qPrintable(filename));
            this->_save_as_dialogue->reject();
            return;
        }
        Q_CHECK_PTR(this->_editor);

        // // Get project data
        QJsonObject global;
        global["packed_externals"] = this->_save_as_pack;
        global["save_version"] = "v1.0";
        json nodeeditor = json::parse(QJsonDocument(this->_editor->save()).toJson().constData());

        // If enable pack external resources (images) within the save file
        if (this->_save_as_pack)
        {
            qInfo("Packing external resources");
            // Loop over all the nodes, if they have a 'image' key, save image to file
            // Update image key to a simple filename
            // TODO: Implement protections for same name
            QuaZipFile image_file(&zip);
            for (int i = 0; i < (int)nodeeditor["nodes"].size(); i++)
            {
                if (nodeeditor["nodes"][i]["model"].contains("image"))
                {
                    // Get the image: /path/to/file.png
                    QString image_path = nodeeditor["nodes"][i]["model"].value("image", "").c_str();

                    // Get filename: file.png
                    QFileInfo file_info(image_path);
                    QString image = file_info.fileName();

                    // Create a new file entry in the zip
                    QuaZipNewInfo image_info(image, image_path);
                    image_info.setPermissions(RW_ALL);
                    if (!image_file.open(QIODevice::WriteOnly, image_info))
                    {
                        qWarning("Unable to open zip file for writing");
                        continue;
                    }

                    // Read the image into byte array
                    QFile file(image_path);
                    if (!file.open(QIODevice::ReadOnly))
                    {
                        qWarning("Unable to open up file for reading");
                        image_file.close();
                        continue;
                    }
                    QByteArray data = file.readAll();

                    // Write data to zip file
                    image_file.write(data);
                    image_file.close();
                    file.close();

                    qDebug("Image compressed and updated: %s", qPrintable(image));

                    // Update the filename reference in the nodes
                    nodeeditor["nodes"][i]["model"]["image"] = image.toStdString();
                }
            }
        }
        Q_CHECK_PTR(this->_save_as_dialogue);

        // Update save file
        global["nodes"] = QJsonDocument::fromJson(QByteArray(nodeeditor.dump().c_str())).object();
        QJsonDocument document(global);

        // Write json data file
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

        // file.write(document.toBinaryData());
        file.write(document.toJson());
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

// Open up the save as dialogue
void MainWindow::saveAs()
{
    Q_CHECK_PTR(this->_save_as_dialogue);
    qDebug("Open saving dialogue");
    this->_save_as_dialogue->show();
}

// Load data from a project file
// TODO: Implement Validator
// TODO: A bit deep, separate into sub functions
// TODO: Add indication for successfully/failing to load
void MainWindow::load()
{
    QString filename = QFileDialog::getOpenFileName(
        nullptr,
        tr("Open Project"),
        QDir::homePath(),
        tr((QString("Project files (*") + QString(EXT) + QString(")")).toStdString().c_str()));

    qInfo("Attempting to load save file: %s", qPrintable(filename));

    if (filename == "")
        return;

    // Load zip file
    QuaZip zip(filename);
    if (!zip.open(QuaZip::mdUnzip))
    {
        qCritical("Unable to unzip save file");
        return;
    }

    // Read data file
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

    // Create json from data file
    QJsonDocument document = QJsonDocument::fromJson(data);
    json settings = json::parse(document.toJson().constData());

    qInfo("Save file version: %s", qPrintable(document["save_version"].toString()));

    // Set some data
    this->_save_as_pack = settings.value("packed_externals", false);

    Q_CHECK_PTR(SETTINGS);

    // Extract images if using a packed save file
    if (this->_save_as_pack)
    {
        qInfo("Extracting packed resources");
        // Extract each image to a temporary directory
        for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile())
        {
            // Skip over data file
            if (zip.getCurrentFileName() == SAVE_DATA_FILE_NAME)
                continue;

            // Read image file
            if (!file.open(QIODevice::ReadOnly))
            {
                qWarning("Unable to read file: %s", qPrintable(zip.getCurrentFileName()));
                continue;
            }
            QByteArray image_file_data = file.readAll();

            // Filename with temp path /tmp/TerrainGenerator_XXXXXX/file.png
            std::string temp_image_file = SETTINGS->tmpDir().path().toStdString() + "/" + zip.getCurrentFileName().toStdString();
            file.close();

            // Write image to temp file
            QFile temp_file(temp_image_file.c_str());
            if (!temp_file.open(QIODevice::WriteOnly))
            {
                qWarning("Unable to write file: %s", temp_image_file.c_str());
                file.close();
                continue;
            }
            qInfo("Extracted file: %s", qPrintable(zip.getCurrentFileName()));

            temp_file.write(image_file_data);
            temp_file.close();
        }

        qInfo("Updating resource references");
        // Update image file references to read temp file location
        for (int i = 0; i < (int)settings["nodes"]["nodes"].size(); i++)
        {
            if (settings["nodes"]["nodes"][i]["model"].contains("image"))
            {
                QString image = settings["nodes"]["nodes"][i]["model"].value("image", "").c_str();

                settings["nodes"]["nodes"][i]["model"]["image"] = SETTINGS->tmpDir().path().toStdString() + "/" + image.toStdString();
            }
        }
    }
    zip.close();

    // Recreate QJson form of json
    document = QJsonDocument::fromJson(QByteArray(settings.dump().c_str()));

    Q_CHECK_PTR(this->_editor);

    // Load nodeeditor
    this->_editor->load(document["nodes"].toObject());

    qDebug("Loading save file complete");
}