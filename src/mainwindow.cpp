#include "mainwindow.h"

#include <QObject>
#include <QDebug>

void MainWindow::setup(Ui::MainWindow *ui)
{
    this->_ui = ui;
    this->_editor = new Nodeeditor(this->_ui->NodeEditorLayout);
    this->_opengl = this->_ui->OpenGLWidget;

    QObject::connect(this->_editor, &Nodeeditor::outputUpdated, this->_opengl, &OpenGL::nodeeditorOutputUpdated);

    QObject::connect(this->_ui->actionSave_As, &QAction::triggered, this, &MainWindow::saveAs);

    this->_ui->splitter_top_bottom->setSizes({300, 150});
}

void MainWindow::saveAs()
{
    qDebug() << "SAVE";
}