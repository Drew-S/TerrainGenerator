#pragma once

#include <nodes/NodeDataModel>

// Customized node class for adding general functionality
class Node : public QtNodes::NodeDataModel
{
    Q_OBJECT
public:
    // When a node is created (not constructor, created into nodeeditor)
    // Used for attaching listeners. paceholder/nodeeditor creates a copy of
    // the node when added to the registry, to attach listeners only when they
    // should be, this is called by my nodeeditor when the node is created
    virtual void created(){};
};