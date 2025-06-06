//
// Created by steph on 5/30/2025.
//

#pragma once
#include <string>
#include <utility>

#include "prefabs/Prefab.h"

namespace Vox
{
    struct SavedWorldObject
    {
        std::string name;
        std::unique_ptr<Prefab> prefab;
    };

    struct SavedWorld
    {
        SavedWorld() = default;
        explicit SavedWorld(const std::string& jsonString);

        std::vector<SavedWorldObject> savedObjects;
    };

} // Vox
