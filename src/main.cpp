#include "ui_Main.h"

#include <QApplication>
#include <QMainWindow>
#include <QDebug>

#include <nodeeditor/include/nodes/FlowScene>
#include <nodeeditor/include/nodes/FlowView>
#include <nodeeditor/include/nodes/DataModelRegistry>

// Register the data models, this is used by the Nodeeditor to create nodes in the editor
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

    // Setup the main window
    ui.setupUi(&main_window);

    // Attach a nodeeditor interface to the widget
    QtNodes::FlowScene *scene = new QtNodes::FlowScene(registerDataModels());
    QtNodes::FlowView *view = new QtNodes::FlowView(scene);

    ui.NodeEditorLayout->addWidget(view);

    // Fix splitter size with the nodeeditor
    ui.splitter_top_bottom->setSizes({300, 150});

    main_window.show();

    return app.exec();
}