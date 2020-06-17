#pragma once

#include <QDialog>
#include <QDir>
#include <QDockWidget>
#include <QMainWindow>

#include "Nodeeditor/nodeeditor.h"
#include "OpenGL/opengl.h"

#include "ui_Main.h"
#include "ui_SaveAsDialogue.h"
#include "ui_Help.h"

/**
 * MainWindow
 * 
 * MainWindow houses the primary interface that the user interacts with. Within
 * this class other classes handles specific widgets. Manages saving and
 * loading, and rendering.
 */
class MainWindow : public QMainWindow
{
public:
    // Setup with the provided UI and attach listeners
    void setup(Ui::MainWindow *ui);

    // Save the current project to the previous file (save) or a new file
    // (saveAs).
    void saveAs();
    // void save();

    // Load a project file from the system.
    void load();

private slots:
    // On save as selected, file dialogue used to select output directory
    void _saveAsFile();

    // When the save as filename is changed user selects filename
    void _saveAsLineEdit(QString const &text);

    // When the accept is pressed we save the file
    void _saveAsAccept();

    // When the pack option is checked we select to pack files together
    void _saveAsTogglePack(bool checked);

private:
    // Information for saving/loading a project file
    QString _save_as_filename = "";
    QString _save_as_directory = QDir::homePath();

    // Containers for main componenets of application
    Nodeeditor *_editor;
    OpenGL *_open_gl;

    // UI for main window and saving
    Ui::MainWindow *_main_ui;
    Ui::SaveAsDialogue *_save_ui;
    Ui::Help _help_ui;

    // Dialogue for save as functions
    QDialog *_save_as_dialogue;
    QDialog *_help;
};