#include "mainwindow.h"

#include <QObject>
#include <QImage>
#include <QDebug>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QRegExp>

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

// Update the filename when the line edit is updated
void MainWindow::_saveAsLineEdit(QString const &text) { this->_save_as_filename = text; }

// Saves the project data to the provided output file
// TODO: Use binary data loading with custom extension
// TODO: ^ Add options for saving with custom name within a custom extension
//       zip file with the option to pack referenced images
void MainWindow::_saveAsAccept()
{
    // Terrain Generator Data File : tgdf
    // TODO: Come up with better extension name
    if (this->_save_as_directory != "" && this->_save_as_filename != "" && this->_save_as_filename != ".tgdf")
    {
        // <file>.tgdf -> <file> (user having tgdf extension is the same as no extension) (prevents <file>.tgdf.tgdf)
        QString filename = this->_save_as_filename;
        filename = filename.replace(QRegExp("\\.tgdf$"), "");

        // Open save as file
        QFile save_file(this->_save_as_directory + "/" + filename + ".tgdf");
        if (!save_file.open(QIODevice::WriteOnly))
        {
            qWarning("Could not open save file");
            return;
        }

        // Get project data
        QJsonObject global;
        global["global_data"] = "stuff";
        global["nodes"] = this->_editor->save();

        QJsonDocument document(global);

        // Write the data to the file
        save_file.write(document.toBinaryData());

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
void MainWindow::load()
{
    QString filename = QFileDialog::getOpenFileName(
        nullptr,
        tr("Open Project"),
        QDir::homePath(),
        tr("Project files (*.tgdf)"));

    if (filename == "")
        return;

    QFile file(filename);
    file.open(QFile::ReadOnly);
    QJsonDocument document = QJsonDocument::fromBinaryData(file.readAll());

    this->_editor->load(document["nodes"].toObject());
}