#include "ui_Main.h"

#include "Nodeeditor/nodeeditor.h"

#include <QApplication>
#include <QMainWindow>
#include <QDebug>

int main(int argc, char *argv[])
{
    qDebug() << "Starting Program";
    QApplication app(argc, argv);

    Ui::MainWindow ui;
    QMainWindow main_window;

    // Setup the main window
    ui.setupUi(&main_window);

    Nodeeditor editor(ui.NodeEditorLayout);

    // Fix splitter size with the nodeeditor
    ui.splitter_top_bottom->setSizes({300, 150});

    main_window.show();

    return app.exec();
}