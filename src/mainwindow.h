#pragma once

#include <QMainWindow>

#include "ui_Main.h"
#include "ui_SaveAsDialogue.h"

#include "Nodeeditor/nodeeditor.h"
#include "OpenGL/opengl.h"

#include <QDialog>
#include <QDir>

// Class that manages the main window, controls save/load from file
class MainWindow : public QMainWindow
{
public:
    MainWindow();
    ~MainWindow();

    void setup(Ui::MainWindow *ui);

    void saveAs();
    // void save();
    void load();

private slots:
    void _saveAsFile();
    void _saveAsLineEdit(QString const &text);
    void _saveAsAccept();

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

    QDialog *_save_as_dialog;
};