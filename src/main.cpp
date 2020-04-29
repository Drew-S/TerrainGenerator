#include "ui_Main.h"

#include <lib/nodeeditor/include/nodes/ConnectionStyle>

#include "mainwindow.h"

#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QObject>

int main(int argc, char *argv[])
{
    qDebug() << "Starting Program";
    QApplication app(argc, argv);

    QtNodes::ConnectionStyle::setConnectionStyle(
        R"(
            "ConnectionStyle": {
                "UseDataDefinedColors": true
            }
        )");

    Ui::MainWindow ui;
    MainWindow main_window;

    // Setup the main window
    ui.setupUi(&main_window);
    main_window.setup(&ui);

    main_window.show();

    return app.exec();
}
