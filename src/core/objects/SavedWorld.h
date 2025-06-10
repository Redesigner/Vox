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
        std::string className;
        std::vector<PropertyOverride> worldContextOverrides;
    };

    struct SavedWorld
    {
        SavedWorld() = default;
        explicit SavedWorld(const std::string& jsonString);

        [[nodiscard]] nlohmann::ordered_json Serialize() const;

        static void CreateOverrides(SavedWorldObject& object, const nlohmann::json& context, const std::vector<std::string>& currentPathStack);

        std::vector<SavedWorldObject> savedObjects;
    };

} // Vox
