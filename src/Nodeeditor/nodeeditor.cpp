#include "nodeeditor.h"

#include <QDebug>

#include <nodeeditor/include/nodes/DataModelRegistry>

// This block of code is used to register all the different nodes
// TODO: Figure out a way for stretch goals to dynamically load models through plugins
static std::shared_ptr<QtNodes::DataModelRegistry> registerDataModels()
{
    std::shared_ptr<QtNodes::DataModelRegistry> registry = std::make_shared<QtNodes::DataModelRegistry>();

    registry->registerModel<OutputNode>();
    registry->registerModel<InputTextureNode>();

    return registry;
}

// Create a node editor manager
Nodeeditor::Nodeeditor(QLayout *target)
{
    // Create a scene and a view, attach models to the scene
    this->_scene = new QtNodes::FlowScene(registerDataModels());
    this->_view = new QtNodes::FlowView(this->_scene);

    // Add the view to the provided target layout (container)
    target->addWidget(this->_view);

    // Connect listeners for node creation and node doubled click
    QObject::connect(this->_scene, &QtNodes::FlowScene::nodeCreated, this, &Nodeeditor::nodeCreated);
    QObject::connect(this->_scene, &QtNodes::FlowScene::nodeDoubleClicked, this, &Nodeeditor::nodeDoubleClicked);
}

// Remove pointers
Nodeeditor::~Nodeeditor()
{
    delete this->_view;
    delete this->_scene;
    delete this->_active_output;
}

// When a node is created, if it is an output node and the current active output node
// is null, set the active output to the newly created output node.
void Nodeeditor::nodeCreated(QtNodes::Node &node)
{
    // Created node is output and active output is null
    if (node.nodeDataModel()->name() == "OutputNode" && this->_active_output == nullptr)
    {
        // Save pointer to the output node
        this->_active_output = dynamic_cast<OutputNode *>(node.nodeDataModel());
        // Connect to computing listeners of the output node
        QObject::connect(this->_active_output, &QtNodes::NodeDataModel::computingStarted, this, &Nodeeditor::outputComputingStarted);
        QObject::connect(this->_active_output, &QtNodes::NodeDataModel::computingFinished, this, &Nodeeditor::outputComputingFinished);
    }
}

// When a node is double clicked, if it is an output node update the active output node
void Nodeeditor::nodeDoubleClicked(QtNodes::Node &node)
{
    // If the node is an output node
    if (node.nodeDataModel()->name() == "OutputNode")
    {
        // Disconnect old listeners
        QObject::disconnect(this->_active_output, &QtNodes::NodeDataModel::computingStarted, this, &Nodeeditor::outputComputingStarted);
        QObject::disconnect(this->_active_output, &QtNodes::NodeDataModel::computingFinished, this, &Nodeeditor::outputComputingFinished);
        // Update the active output node
        this->_active_output = dynamic_cast<OutputNode *>(node.nodeDataModel());
        // Attach new listeners
        QObject::connect(this->_active_output, &QtNodes::NodeDataModel::computingStarted, this, &Nodeeditor::outputComputingStarted);
        QObject::connect(this->_active_output, &QtNodes::NodeDataModel::computingFinished, this, &Nodeeditor::outputComputingFinished);
    }
}

// When the output node begins calculating
// TODO: Have this start a busy indicator
void Nodeeditor::outputComputingStarted() {}

// When the output is done emit that the generated normal map is done
// TODO: Have this complete a busy indicator
void Nodeeditor::outputComputingFinished()
{
    // Inform parents that there are new normal and height maps (main.cpp)
    emit this->outputUpdated(this->getNormalMap(), this->getHeightMap());
}

// Returns the active output node height map or an empty QImage
QImage Nodeeditor::getHeightMap()
{
    if (this->_active_output)
        return this->_active_output->getHeightMap();
    else
        return QImage();
}

// Returns the active output node height map or an blue QImage (normal compensated)
QImage Nodeeditor::getNormalMap()
{
    if (this->_active_output)
    {
        return this->_active_output->getNormalMap();
    }
    else
    {
        QImage normal(1, 1, QImage::Format_Indexed8);
        normal.setColorCount(1);
        normal.setColor(0, qRgba(128, 128, 255, 255));
        normal.fill(0);
        return normal;
    }
}