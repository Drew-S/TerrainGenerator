#pragma once

#include <QPixmap>

#include <nodeeditor/include/nodes/NodeDataModel>

// Manages the sharing of a pixmap between nodes
// Used to send once image from one node to another
class PixmapData : public QtNodes::NodeData
{
public:
    PixmapData(){};
    PixmapData(QPixmap const &pixmap) : _pixmap(pixmap){};

    // The type of data this is { QString: id, QString name}, name shows up on node
    QtNodes::NodeDataType type() const override { return {"pixmap", "sampler2D"}; };

    // Returns the stored pixmap
    QPixmap pixmap() const { return this->_pixmap; };

private:
    QPixmap _pixmap;
};