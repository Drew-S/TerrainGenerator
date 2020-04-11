#include "ui_Main.h"

#include <QApplication>
#include <QMainWindow>
#include <QDebug>

int main(int argc, char *argv[])
{
    qDebug() << "Starting Program";
    QApplication app(argc, argv);

    Ui::MainWindow ui;
    QMainWindow main_window;

    ui.setupUi(&main_window);

    main_window.show();

    return app.exec();
}