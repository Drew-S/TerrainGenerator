#pragma once

#include <nodes/NodeDataModel>

/**
 * Node
 * 
 * Subclass of NodeDataModel that is a superclass of all other nodes in this
 * folder. This defines extra functions for attaching listeners with created,
 * checks if the node has a shared widget for the properties panel, and gets
 * the shared proprties widget.
 */
class Node : public QtNodes::NodeDataModel
{
    Q_OBJECT
public:
    // When a node is created (not constructor, created into nodeeditor)
    // Used for attaching listeners. paceholder/nodeeditor creates a copy of
    // the node when added to the registry, to attach listeners only when they
    // should be, this is called by my nodeeditor when the node is created
    virtual void created(){};
    virtual bool hasShared() { return false; };
    virtual QWidget *sharedWidget() { return nullptr; };
};