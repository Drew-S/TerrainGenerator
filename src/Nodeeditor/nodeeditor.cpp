#include "nodeeditor.h"

#include <QDebug>
#include <QJsonDocument>
#include <QHBoxLayout>

#include <nodes/DataModelRegistry>
#include <nodes/TypeConverter>

#include "./Datatypes/pixmap.h"
#include "./Datatypes/converters.h"

#define CAST_NODE(NODE)                         \
    NODE *selected = static_cast<NODE *>(node); \
    QWidget *shared = selected->sharedWidget(); \
    this->_updatePropertieNodesShared(shared);

// This block of code is used to register all the different nodes
// TODO: Figure out a way for stretch goals to dynamically load models through plugins
static std::shared_ptr<QtNodes::DataModelRegistry> registerDataModels()
{
    std::shared_ptr<QtNodes::DataModelRegistry> registry = std::make_shared<QtNodes::DataModelRegistry>();
    qDebug("Registering Data Models and Data Converters");

    registry->registerModel<OutputNode>("Output");
    registry->registerModel<InputTextureNode>("Input");
    registry->registerModel<InputSimplexNoiseNode>("Input");
    registry->registerModel<ConverterMathNode>("Converters");
    registry->registerModel<ConverterColorSplitNode>("Converters");
    registry->registerModel<ConverterColorCombineNode>("Converters");
    registry->registerModel<ConverterVectorDotNode>("Converters");
    registry->registerModel<ConverterVectorIntensityNode>("Converters");
    registry->registerModel<ConverterVectorMathNode>("Converters");

    // Converters to automatically convert IntensityMap <-> VectorMap data between nodes
    registry->registerTypeConverter(std::make_pair(
                                        VectorMapData().type(),
                                        IntensityMapData().type()),
                                    QtNodes::TypeConverter{VectorToIntensityMapConverter()});

    registry->registerTypeConverter(std::make_pair(
                                        IntensityMapData().type(),
                                        VectorMapData().type()),
                                    QtNodes::TypeConverter{IntensityToVectorMapConverter()});

    return registry;
}

// Create a node editor manager
Nodeeditor::Nodeeditor(QLayout *target, QWidget *properties)
{
    Q_CHECK_PTR(target);
    Q_CHECK_PTR(properties);
    qDebug("Setting up nodeeditor widget");
    // Create a scene and a view, attach models to the scene
    this->_scene = new QtNodes::FlowScene(registerDataModels());
    this->_view = new QtNodes::FlowView(this->_scene);
    this->_view->setSceneRect(-32767, -32726, 32727 * 2, 327267 * 2);
    this->_properties = properties;
    this->_properties->setLayout(new QHBoxLayout());

    // Add the view to the provided target layout (container)
    target->addWidget(this->_view);

    // Connect listeners for node creation and node doubled click
    QObject::connect(this->_scene, &QtNodes::FlowScene::nodeCreated, this, &Nodeeditor::nodeCreated);
    QObject::connect(this->_scene, &QtNodes::FlowScene::nodeDoubleClicked, this, &Nodeeditor::nodeDoubleClicked);
    QObject::connect(this->_scene, &QtNodes::FlowScene::nodeDeleted, this, &Nodeeditor::nodeDeleted);
}

// Remove pointers
Nodeeditor::~Nodeeditor()
{
    Q_CHECK_PTR(this->_view);
    Q_CHECK_PTR(this->_scene);
    Q_CHECK_PTR(this->_active_output);
    delete this->_view;
    delete this->_scene;
    delete this->_active_output;
}

// Swap out or replace the node's property node
void Nodeeditor::_updatePropertieNodesShared(QWidget *shared)
{
    if (this->_properties_node)
    {
        this->_properties->layout()->removeWidget(this->_properties_node);
        this->_properties_node->hide();
        this->_properties_node->setParent(nullptr);
    }

    if (shared)
    {
        this->_properties->layout()->addWidget(shared);
        this->_properties_node = shared;
        this->_properties_node->show();
    }
    else
    {
        this->_properties_node = nullptr;
    }
}

// Handler for placing shared widget in the properties container if the node supports it
void Nodeeditor::_updatePropertiesNode(QtNodes::NodeDataModel *node, bool swap)
{
    Q_CHECK_PTR(node);
    QString name = node->name();
    if (name == InputSimplexNoiseNode().name())
    {
        CAST_NODE(InputSimplexNoiseNode)
    }
    else if (name == InputTextureNode().name())
    {
        CAST_NODE(InputTextureNode)
    }
    else if (name == ConverterMathNode().name())
    {
        CAST_NODE(ConverterMathNode)
    }
    else if (name == ConverterColorCombineNode().name())
    {
        CAST_NODE(ConverterColorCombineNode)
    }
    else if (name == ConverterVectorDotNode().name())
    {
        CAST_NODE(ConverterVectorDotNode)
    }
    else if (name == ConverterVectorIntensityNode().name())
    {
        CAST_NODE(ConverterVectorIntensityNode)
    }
    else if (name == ConverterVectorMathNode().name())
    {
        CAST_NODE(ConverterVectorMathNode)
    }
    else if (swap)
    {
        this->_properties->layout()->removeWidget(this->_properties_node);
        this->_properties_node->hide();
        this->_properties_node->setParent(nullptr);
        this->_properties_node = nullptr;
    }
}

// When a node is created, if it is an output node and the current active output node
// is null, set the active output to the newly created output node.
void Nodeeditor::nodeCreated(QtNodes::Node &node)
{
    QString name = node.nodeDataModel()->name();
    // Created node is output and active output is null
    if (name == OutputNode().name() && !this->_active_output)
    {
        qDebug("Setting active output node");
        // Save pointer to the output node
        this->_active_output = static_cast<OutputNode *>(node.nodeDataModel());

        // Connect to computing listeners of the output node
        QObject::connect(this->_active_output, &QtNodes::NodeDataModel::computingStarted, this, &Nodeeditor::outputComputingStarted);
        QObject::connect(this->_active_output, &QtNodes::NodeDataModel::computingFinished, this, &Nodeeditor::outputComputingFinished);
    }
    this->_updatePropertiesNode(node.nodeDataModel());
}

// When a node is double clicked, if it is an output node update the active output node
void Nodeeditor::nodeDoubleClicked(QtNodes::Node &node)
{
    QString name = node.nodeDataModel()->name();
    // If the node is an output node
    if (name == OutputNode().name())
    {
        qDebug("Updating active output node");
        // Disconnect old listeners
        if (this->_active_output)
        {
            QObject::disconnect(this->_active_output, &QtNodes::NodeDataModel::computingStarted, this, &Nodeeditor::outputComputingStarted);
            QObject::disconnect(this->_active_output, &QtNodes::NodeDataModel::computingFinished, this, &Nodeeditor::outputComputingFinished);
        }
        // Update the active output node
        this->_active_output = static_cast<OutputNode *>(node.nodeDataModel());
        // Attach new listeners
        QObject::connect(this->_active_output, &QtNodes::NodeDataModel::computingStarted, this, &Nodeeditor::outputComputingStarted);
        QObject::connect(this->_active_output, &QtNodes::NodeDataModel::computingFinished, this, &Nodeeditor::outputComputingFinished);
    }
    this->_updatePropertiesNode(node.nodeDataModel(), true);
}

// When the output node begins calculating
// TODO: Have this start a busy indicator
void Nodeeditor::outputComputingStarted() {}

// When the output is done emit that the generated normal map is done
// TODO: Have this complete a busy indicator
void Nodeeditor::outputComputingFinished()
{
    qDebug("Output node done computing normal map");
    // Inform parents that there are new normal and height maps (main.cpp)
    emit this->outputUpdated(this->getNormalMap(), this->getHeightMap());
}

void Nodeeditor::nodeDeleted(QtNodes::Node &node)
{
    Q_UNUSED(node);
    this->_updatePropertieNodesShared(nullptr);
}

// Returns the active output node height map or an empty QImage
QImage Nodeeditor::getHeightMap()
{
    if (this->_active_output)
    {
        OutputNode *node = static_cast<OutputNode *>(this->_active_output);
        return node->getHeightMap();
    }
    else
    {
        return QImage();
    }
}

// Returns the active output node height map or an blue QImage (normal compensated)
QImage Nodeeditor::getNormalMap()
{
    if (this->_active_output)
    {
        OutputNode *node = static_cast<OutputNode *>(this->_active_output);
        return node->getNormalMap();
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

// Saves the nodeeditor to a save file
QJsonObject Nodeeditor::save()
{
    qDebug("Saving dataflow diagram");
    return QJsonDocument::fromJson(this->_scene->saveToMemory()).object();
}

// Loads the nodeeditor from a save file
void Nodeeditor::load(QJsonObject data)
{
    qDebug("Loading dataflow diagram");
    this->_scene->loadFromMemory(QJsonDocument(data).toJson());
}
