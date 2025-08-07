//
// Created by steph on 5/30/2025.
//

#pragma once
#include "../properties/Property.h"

namespace Vox
{
    /**
     * @brief Holds information about property changes made by the user to a prefab
     */
    struct PropertyOverride
    {
        std::string path;
        std::string propertyName;
        TypedPropertyVariant variant;
    };
} // Vox
