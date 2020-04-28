#pragma once

#include <QPixmap>

#include <lib/nodeeditor/include/nodes/NodeDataModel>

#include "structures.h"

// Manages the sharing of a pixmap between nodes
// Used to send once image from one node to another
class PixmapData : public QtNodes::NodeData
{
public:
    PixmapData(){};
    PixmapData(QPixmap const &pixmap)
    {
        this->_vector_map = VectorMap(pixmap);
    };
    PixmapData(VectorMap const &vector_map) : _vector_map(vector_map){};

    // The type of data this is { QString: id, QString name}, name shows up on node
    QtNodes::NodeDataType type() const override { return {"PixmapData", "sampler2D"}; };

    // Returns the stored colorMap
    VectorMap vectorMap() const { return this->_vector_map; };

private:
    VectorMap _vector_map;
};