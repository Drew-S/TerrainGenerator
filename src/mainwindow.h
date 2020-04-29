#pragma once

#include <QMainWindow>

#include "ui_Main.h"

#include "Nodeeditor/nodeeditor.h"
#include "OpenGL/opengl.h"

class MainWindow : public QMainWindow
{
public:
    MainWindow(){};
    ~MainWindow(){};

    void setup(Ui::MainWindow *ui);

    void saveAs();
    // void save();
    // void load();

private:
    Nodeeditor *_editor;
    OpenGL *_opengl;
    Ui::MainWindow *_ui;
};