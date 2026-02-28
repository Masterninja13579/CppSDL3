#pragma once

#include "core.h"

struct Tab
{
    int source = -1;
    int index = -1;

    inline Tab() {}
    inline Tab(int source, int index)
        : source(source), index(index)
    {

    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Tab,
    source,
    index
)
