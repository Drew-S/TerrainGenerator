#pragma once

#include <vector>

#include "stencil.h"

class StencilList
{
public:
    static StencilList *getInstance();
    ~StencilList();
    Stencil *stencil(int index);
    int count();

private:
    StencilList();
    static bool _instance;
    static StencilList *_single;
    std::vector<Stencil> _stencils;
};