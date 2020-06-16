#include "nodeeditor.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QJsonDocument>

#include <nodes/DataModelRegistry>
#include <nodes/TypeConverter>

#include "./Datatypes/converters.h"
#include "./Datatypes/pixmap.h"
#include "./Nodes/node.h"

// Helper macro
// TODO: With new node.h superclass this is now overly complex, replace.
#define CAST_NODE(NODE)                         \
    NODE *selected = static_cast<NODE *>(node); \
    QWidget *shared = selected->sharedWidget(); \
    this->_updatePropertieNodesShared(shared);

/**
 * registerDataModels
 * 
 * This function creates a registry that is used with QtNodes to define the
 * nodes that exist in the dataflow diagram.
 * 
 * @returns std::shared_ptr<QtNodes::DataModelRegistry> : The registry.
 * 
 * TODO: Figure out a way for stretch goals to dynamically load models through plugins
 */
static std::shared_ptr<QtNodes::DataModelRegistry> registerDataModels()
{
    std::shared_ptr<QtNodes::DataModelRegistry> registry =
        std::make_shared<QtNodes::DataModelRegistry>();

    qDebug("Registering Data Models and Data Converters");

    registry->registerModel<OutputNode>("Output");

    registry->registerModel<InputTextureNode>("Input");
    registry->registerModel<InputSimplexNoiseNode>("Input");
    registry->registerModel<InputConstantValueNode>("Input");
    registry->registerModel<InputConstantVectorNode>("Input");

    registry->registerModel<ConverterMathNode>("Converters");
    registry->registerModel<ConverterColorSplitNode>("Converters");
    registry->registerModel<ConverterColorCombineNode>("Converters");
    registry->registerModel<ConverterVectorDotNode>("Converters");
    registry->registerModel<ConverterVectorIntensityNode>("Converters");
    registry->registerModel<ConverterVectorMathNode>("Converters");
    registry->registerModel<ConverterInvertIntensityNode>("Converters");
    registry->registerModel<ConverterNormalizeNode>("Converters");
    registry->registerModel<ConverterBezierCurveNode>("Converters");
    registry->registerModel<ConverterClampNode>("Converters");
    registry->registerModel<ConverterErosionNode>("Converters");
    registry->registerModel<ConverterSmoothNode>("Converters");

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

/**
 * Nodeeditor
 * 
 * Creates a new nodeeditor with the target layout to place the flow view and
 * the target widget for displaying properties.
 * 
 * @param QLayout* target : The layout to place the flowview onto.
 * @param QWidget* properties : The widget to display sharedWidget on.
 */
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

/**
 * ~Nodeeditor
 * 
 * Delete the node editor and its managed QtNode items.
 */
Nodeeditor::~Nodeeditor()
{
    Q_CHECK_PTR(this->_view);
    Q_CHECK_PTR(this->_scene);
    Q_CHECK_PTR(this->_active_output);
    delete this->_view;
    delete this->_scene;
    delete this->_active_output;
}

/**
 * _updatePropertiesNodesShared
 * 
 * Sets/replaces the properties dispayed in the side panel. If the pointer is
 * null, it only removes and hides the properties panel.
 * 
 * @param QWidget *shared : The widget to be displayed or nullptr to simply hide
 *                          the panel.
 */
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

/**
 * _updatePropertiesNode
 * 
 * Takes the input node data model and a flag and casts and gets the widget for
 * displaying on the properties panel.
 * 
 * @param QtNodes::NodeDataModel* node : Pointer to the node to get properties
 *                                       widget for.
 * @param bool swap : Whether we are swapping existing properties or creating a
 *                    new one.
 * 
 * TODO: Use new node.h method and remove CASTS
 */
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
    else if (name == InputConstantValueNode().name())
    {
        CAST_NODE(InputConstantValueNode)
    }
    else if (name == InputConstantVectorNode().name())
    {
        CAST_NODE(InputConstantVectorNode)
    }
    else if (name == ConverterBezierCurveNode().name())
    {
        CAST_NODE(ConverterBezierCurveNode)
    }
    else if (name == ConverterClampNode().name())
    {
        CAST_NODE(ConverterClampNode)
    }
    else if (name == ConverterErosionNode().name())
    {
        CAST_NODE(ConverterErosionNode)
    }
    else if (swap)
    {
        this->_properties->layout()->removeWidget(this->_properties_node);
        this->_properties_node->hide();
        this->_properties_node->setParent(nullptr);
        this->_properties_node = nullptr;
    }
}

/**
 * nodeCreated @slot
 * 
 * When a new node is created and placed into the scene. If the model is an
 * output node we set the active output node and then we get any properties
 * widget the node has.
 * 
 * @param QtNodes::Node& node : The node that was created.
 */
void Nodeeditor::nodeCreated(QtNodes::Node &node)
{
    QString name = node.nodeDataModel()->name();
    Node *created_node = static_cast<Node *>(node.nodeDataModel());
    created_node->created();
    // Created node is output and active output is null
    if (name == OutputNode().name() && !this->_active_output)
    {
        qDebug("Setting active output node");
        // Save pointer to the output node
        this->_active_output = static_cast<OutputNode *>(node.nodeDataModel());

        // Connect to computing listeners of the output node
        QObject::connect(this->_active_output,
                         &QtNodes::NodeDataModel::computingFinished,
                         this,
                         &Nodeeditor::outputComputingFinished);
    }
    this->_updatePropertiesNode(node.nodeDataModel());
}

/**
 * nodeDoubleClicked @slot
 * 
 * When a node is double clicked we set it as the active node (if output) and
 * update the properties panel with the new nodes widget.
 * 
 * @param QtNodes::Node& node : The node that was double clicked.
 */
void Nodeeditor::nodeDoubleClicked(QtNodes::Node &node)
{
    QString name = node.nodeDataModel()->name();
    // If the node is an output node
    if (name == OutputNode().name())
    {
        qDebug("Updating active output node");
        // Disconnect old listeners
        if (this->_active_output)
            QObject::disconnect(this->_active_output,
                                &QtNodes::NodeDataModel::computingFinished,
                                this,
                                &Nodeeditor::outputComputingFinished);

        // Update the active output node
        this->_active_output = static_cast<OutputNode *>(node.nodeDataModel());
        // Attach new listeners
        QObject::connect(this->_active_output,
                         &QtNodes::NodeDataModel::computingFinished,
                         this,
                         &Nodeeditor::outputComputingFinished);
    }
    this->_updatePropertiesNode(node.nodeDataModel(), true);
}

/**
 * outputComputingFinished @slot
 * 
 * When the output node has completed its computation (the sum of the dataflow)
 * we emit changes to be displayed in the opengl widget.
 * 
 * @signals outputUpdated
 */
void Nodeeditor::outputComputingFinished()
{
    qDebug("Output node done computing normal map");
    // Inform parents that there are new normal and height maps (main.cpp)
    emit this->outputUpdated(this->getNormalMap(),
                             this->getHeightMap(),
                             this->getAlbedoMap());
}

/**
 * nodeDeleted @slot
 * 
 * When a node is deleted this slot updates the properties panel removing any
 * current attached widget.
 * 
 * @param QtNodes::Node& node : Not used in this function.
 */
void Nodeeditor::nodeDeleted(QtNodes::Node &node)
{
    Q_UNUSED(node);
    this->_updatePropertieNodesShared(nullptr);
}

/**
 * getHeightMap
 * 
 * Get the generated height map, if the height map is not set we return a blank
 * image.
 * 
 * @returns QImage : The height map.
 */
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

/**
 * getNormalMap
 * 
 * Get the generated normal map, if the normal map is not set we return a blank
 * image.
 * 
 * @returns QImage : The normal map.
 */
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

/**
 * getAlbedoMap
 * 
 * Get the supplied albedo map, if the albedo map is not set we return a blank
 * image.
 * 
 * @returns QImage : The albedo map.
 */
QImage Nodeeditor::getAlbedoMap()
{
    if (this->_active_output)
    {
        OutputNode *node = static_cast<OutputNode *>(this->_active_output);
        return node->getAlbedoMap();
    }
    else
    {
        QImage albedo(1, 1, QImage::Format_Indexed8);
        albedo.setColorCount(1);
        albedo.setColor(0, qRgba(200, 200, 200, 255));
        albedo.fill(0);
        return albedo;
    }
}

/**
 * save
 * 
 * Returns the QJsonObject version of the obtained raw byte data for the
 * dataflow diagrams nodes. This is used to save to a project file.
 * 
 * @returns QJsonObject : The combined node data.
 */
QJsonObject Nodeeditor::save()
{
    qDebug("Saving dataflow diagram");
    return QJsonDocument::fromJson(this->_scene->saveToMemory()).object();
}

/**
 * load
 * 
 * Loads the dataflow diagram from a json object.
 * 
 * @param QJsonObject data : The data to load from.
 */
void Nodeeditor::load(QJsonObject data)
{
    qDebug("Loading dataflow diagram");
    this->_scene->loadFromMemory(QJsonDocument(data).toJson());
}
