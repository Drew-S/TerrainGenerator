#pragma once

#include <QLayout>

#include <nodeeditor/include/nodes/FlowScene>
#include <nodeeditor/include/nodes/FlowView>

class Nodeeditor
{
public:
    Nodeeditor(QLayout *target);
    ~Nodeeditor();

private:
    QtNodes::FlowScene *_scene;
    QtNodes::FlowView *_view;
};