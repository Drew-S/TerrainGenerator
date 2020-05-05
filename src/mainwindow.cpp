#include "mainwindow.h"

#include <string>

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

#define SAVE_DATA_FILE_NAME "data.json"
#define RW_ALL QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadUser | QFileDevice::WriteUser | QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ReadOther | QFileDevice::WriteOther

MainWindow::MainWindow()
{
    this->_packed_files = new QTemporaryDir(QDir::tempPath() + "/TerrainGenerator_XXXXXX");
}
MainWindow::~MainWindow()
{
    delete this->_packed_files;
}

// Setup is a function used to setup the application,
// Connect to ui elements and controls and link sub components
void MainWindow::setup(Ui::MainWindow *ui)
{
    this->_main_ui = ui;
    this->_editor = new Nodeeditor(ui->NodeEditorLayout);
    this->_open_gl = this->_main_ui->OpenGLWidget;

    // Connect output widget compute finished to OpenGL to display results
    QObject::connect(this->_editor, &Nodeeditor::outputUpdated, this->_open_gl, &OpenGL::nodeeditorOutputUpdated);

    // Connect file actions to load and save files
    QObject::connect(this->_main_ui->actionSave_As, &QAction::triggered, this, &MainWindow::saveAs);
    QObject::connect(this->_main_ui->actionLoad, &QAction::triggered, this, &MainWindow::load);

    // Fix Nodeeditor and OpenGL widget splitter size
    this->_main_ui->splitter_top_bottom->setSizes({300, 150});

    // Create a save as dialog and setup it ui
    this->_save_as_dialog = new QDialog(this);

    this->_save_ui = new Ui::SaveAsDialogue();
    this->_save_ui->setupUi(this->_save_as_dialog);

    // Set output directory default label
    this->_save_ui->directory_label->setText(QDir::homePath());

    // Connect directory to select output directory
    QObject::connect(this->_save_ui->directory_button, &QPushButton::clicked, this, &MainWindow::_saveAsFile);

    // Connect line edit to define filename output (extension is automatic)
    QObject::connect(this->_save_ui->filename_line_edit, &QLineEdit::textChanged, this, &MainWindow::_saveAsLineEdit);

    // Connect the ok button to save as to write file. Connect cancel button to close the dialogue
    QObject::connect(this->_save_ui->button_box, &QDialogButtonBox::accepted, this, &MainWindow::_saveAsAccept);
    QObject::connect(this->_save_ui->button_box, &QDialogButtonBox::rejected, this->_save_as_dialog, &QDialog::reject);
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

    this->_save_ui->directory_label->setText(directory);
    this->_save_as_directory = directory;
}

// Toggle whether or not to pack images with save file
void MainWindow::_saveAsTogglePack(bool checked)
{
    this->_save_as_pack = checked;
}

// Update the filename when the line edit is updated
void MainWindow::_saveAsLineEdit(QString const &text)
{
    this->_save_as_filename = text;
}

// Saves the project data to the provided output file
// TODO: A bit deep, separate into sub functions
void MainWindow::_saveAsAccept()
{
    // Terrain Generator Data File : tgdf
    // TODO: Come up with better extension name
    if (this->_save_as_directory != "" && this->_save_as_filename != "" && this->_save_as_filename != ".tgdf")
    {
        // <file>.tgdf -> <file> (user having tgdf extension is the same as no extension) (prevents <file>.tgdf.tgdf)
        QString filename = this->_save_as_filename;
        filename = filename.replace(QRegExp("\\.tgdf$"), "");
        filename = this->_save_as_directory + "/" + filename + ".tgdf";

        QuaZip zip(filename);
        if (!zip.open(QuaZip::mdAdd))
        {
            this->_save_as_dialog->reject();
            return;
        }

        // // Get project data
        QJsonObject global;
        global["packed_externals"] = this->_save_as_pack;
        global["save_version"] = "v1.0";
        json nodeeditor = json::parse(QJsonDocument(this->_editor->save()).toJson().constData());

        // If enable pack external resources (images) within the save file
        if (this->_save_as_pack)
        {
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
                        continue;

                    // Read the image into byte array
                    QFile file(image_path);
                    if (!file.open(QIODevice::ReadOnly))
                    {
                        image_file.close();
                        continue;
                    }
                    QByteArray data = file.readAll();

                    // Write data to zip file
                    image_file.write(data);
                    image_file.close();
                    file.close();

                    // Update the filename reference in the nodes
                    nodeeditor["nodes"][i]["model"]["image"] = image.toStdString();
                }
            }
        }

        // Update save file
        global["nodes"] = QJsonDocument::fromJson(QByteArray(nodeeditor.dump().c_str())).object();
        QJsonDocument document(global);

        // Write json data file
        QuaZipFile file(&zip);
        QuaZipNewInfo info(SAVE_DATA_FILE_NAME);
        info.setPermissions(RW_ALL);
        if (!file.open(QIODevice::WriteOnly, info))
        {
            zip.close();
            this->_save_as_dialog->reject();
            return;
        }

        // file.write(document.toBinaryData());
        file.write(document.toJson());
        file.close();
        zip.close();

        // Close the dialogue
        this->_save_as_dialog->accept();
    }
    else
    {
        this->_save_as_dialog->reject();
    }
}

// Open up the save as dialogue
void MainWindow::saveAs()
{
    this->_save_as_dialog->exec();
}

// Load data from a project file
// TODO: Implement Validator
// TODO: A bit deep, separate into sub functions
void MainWindow::load()
{
    QString filename = QFileDialog::getOpenFileName(
        nullptr,
        tr("Open Project"),
        QDir::homePath(),
        tr("Project files (*.tgdf)"));

    if (filename == "")
        return;

    // Load zip file
    QuaZip zip(filename);
    if (!zip.open(QuaZip::mdUnzip))
        return;

    // Read data file
    zip.setCurrentFile(SAVE_DATA_FILE_NAME);
    QuaZipFile file(&zip);

    if (!file.open(QIODevice::ReadOnly))
    {
        zip.close();
        return;
    }
    QByteArray data = file.readAll();
    file.close();

    // Create json from data file
    QJsonDocument document = QJsonDocument::fromJson(data);
    json settings = json::parse(document.toJson().constData());

    // Set some data
    this->_save_as_pack = settings.value("packed_externals", false);

    // Extract images if using a packed save file
    if (this->_save_as_pack)
    {
        // Extract each image to a temporary directory
        for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile())
        {
            // Skip over data file
            if (zip.getCurrentFileName() == SAVE_DATA_FILE_NAME)
                continue;

            // Read image file
            if (!file.open(QIODevice::ReadOnly))
                continue;
            QByteArray image_file_data = file.readAll();

            // Get filename
            QuaZipFileInfo image_info;
            zip.getCurrentFileInfo(&image_info);

            // Filename with temp path /tmp/TerrainGenerator_XXXXXX/file.png
            std::string temp_image_file = this->_packed_files->path().toStdString() + "/" + image_info.name.toStdString();
            file.close();

            // Write image to temp file
            QFile temp_file(temp_image_file.c_str());
            if (!temp_file.open(QIODevice::WriteOnly))
            {
                file.close();
                continue;
            }

            temp_file.write(image_file_data);
            temp_file.close();
        }

        // Update image file references to read temp file location
        for (int i = 0; i < (int)settings["nodes"]["nodes"].size(); i++)
        {
            if (settings["nodes"]["nodes"][i]["model"].contains("image"))
            {
                QString image = settings["nodes"]["nodes"][i]["model"].value("image", "").c_str();

                settings["nodes"]["nodes"][i]["model"]["image"] = this->_packed_files->path().toStdString() + "/" + image.toStdString();
            }
        }
    }
    zip.close();

    // Recreate QJson form of json
    document = QJsonDocument::fromJson(QByteArray(settings.dump().c_str()));

    // Load nodeeditor
    this->_editor->load(document["nodes"].toObject());
}