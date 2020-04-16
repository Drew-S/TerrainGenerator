#include "ui_Main.h"

#include <QApplication>
#include <QMainWindow>
#include <QDebug>

#include <nodeeditor/include/nodes/FlowScene>
#include <nodeeditor/include/nodes/FlowView>
#include <nodeeditor/include/nodes/DataModelRegistry>
// #include <nodes/FlowScene>
// #include <nodes/FlowView>
// #include <nodes/DataModelRegistry>

static std::shared_ptr<QtNodes::DataModelRegistry> registerDataModels()
{
    std::shared_ptr<QtNodes::DataModelRegistry> registry = std::make_shared<QtNodes::DataModelRegistry>();
    return registry;
}

int main(int argc, char *argv[])
{
    qDebug() << "Starting Program";
    QApplication app(argc, argv);

    Ui::MainWindow ui;
    QMainWindow main_window;

    ui.setupUi(&main_window);

    QtNodes::FlowScene *scene = new QtNodes::FlowScene(registerDataModels());
    QtNodes::FlowView *view = new QtNodes::FlowView(scene);

    ui.NodeEditorLayout->addWidget(view);

    main_window.show();

    return app.exec();
}