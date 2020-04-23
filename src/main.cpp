#include "ui_Main.h"

#include "Nodeeditor/nodeeditor.h"
#include "OpenGL/opengl.h"

#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QObject>

int main(int argc, char *argv[])
{
    qDebug() << "Starting Program";
    QApplication app(argc, argv);

    Ui::MainWindow ui;
    QMainWindow main_window;

    // Setup the main window
    ui.setupUi(&main_window);

    // Manager for the Nodes flow and scene
    Nodeeditor editor(ui.NodeEditorLayout);

    // Connect to the nodeeditor output update signal
    QObject::connect(
        &editor,
        &Nodeeditor::outputUpdated,
        ui.OpenGLWidget,
        &OpenGL::nodeeditorOutputUpdated);

    // Fix splitter size with the nodeeditor
    ui.splitter_top_bottom->setSizes({300, 150});

    main_window.show();

    return app.exec();
}