//
// Created by steph on 5/30/2025.
//

#pragma once
#include "Property.h"

namespace Vox
{
    struct PropertyOverride
    {
        std::vector<std::string> path;
        std::string propertyName;
        TypedPropertyVariant variant;
    };
} // Vox
