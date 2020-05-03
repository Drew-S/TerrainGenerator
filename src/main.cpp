#include "ui_Main.h"

#include <lib/nodeeditor/include/nodes/ConnectionStyle>

#include "mainwindow.h"

#include "Nodeeditor/nodeeditor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    qDebug() << "Starting Program";
    QApplication app(argc, argv);

    // Should colorize the connections between nodes based on their data type
    // generated from the name of the data type (to fully customize I would
    // need to change nodeeditor itself, which I may yet do)
    QtNodes::ConnectionStyle::setConnectionStyle(
        R"(
            "ConnectionStyle": {
                "UseDataDefinedColors": true
            }
        )");

    // Load ui for main window and attach it to custom QMainWindow
    Ui::MainWindow ui;
    MainWindow main_window;

    // Setup the main window
    ui.setupUi(&main_window);
    main_window.setup(&ui);

    main_window.show();

    return app.exec();
}
