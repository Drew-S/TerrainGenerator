#include "nodeeditor.h"

#include <nodeeditor/include/nodes/DataModelRegistry>

#include "Nodes/output.h"
#include "Nodes/inputtexture.h"

static std::shared_ptr<QtNodes::DataModelRegistry> registerDataModels()
{
    std::shared_ptr<QtNodes::DataModelRegistry> registry = std::make_shared<QtNodes::DataModelRegistry>();

    registry->registerModel<OutputNode>();
    registry->registerModel<InputTextureNode>();

    return registry;
}

Nodeeditor::Nodeeditor(QLayout *target)
{
    this->_scene = new QtNodes::FlowScene(registerDataModels());
    this->_view = new QtNodes::FlowView(this->_scene);

    target->addWidget(this->_view);
}

Nodeeditor::~Nodeeditor()
{
    delete this->_view;
    delete this->_scene;
}