#include "stencillist.h"

#include <QDir>
#include <QStringList>
#include <QDebug>

bool StencilList::_instance = false;
StencilList *StencilList::_single = nullptr;

StencilList::StencilList()
{
    QDir stencils("assets/stencils");
    QStringList images = stencils.entryList(QStringList() << "*.jpg"
                                                          << "*.png"
                                                          << "*.jpeg",
                                            QDir::Files);

    qDebug("Initializing stencil brushes");
    for (int i = 0; i < images.size(); i++)
    {
        this->_stencils.push_back(Stencil(QString("assets/stencils/") + images.at(i), i));
    }
}

StencilList::~StencilList()
{
    _instance = false;
}

Stencil *StencilList::stencil(int index)
{
    if (index < 0 || index >= this->count())
        return nullptr;

    return &this->_stencils[index];
}

int StencilList::count()
{
    return (int)this->_stencils.size();
}

StencilList *StencilList::getInstance()
{
    if (!_instance)
    {
        _instance = true;
        _single = new StencilList();
    }
    return _single;
}