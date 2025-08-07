//
// Created by steph on 5/30/2025.
//

#pragma once
#include <string>
#include <utility>

#include "../prefabs/Prefab.h"

namespace Vox
{
    /**
     * @brief Holds information about an object in the world
     */
    struct SavedWorldObject
    {
        /// Name of the object, to be identified in the world outline panel
        std::string name;

        /// Name of the class, used to Identify the relevant ObjectClass
        std::string className;

        /// Any properties that might have been changed by the user
        std::vector<PropertyOverride> worldContextOverrides;
    };

    struct SavedWorld
    {
        SavedWorld() = default;
        explicit SavedWorld(const std::string& jsonString);

        [[nodiscard]] nlohmann::ordered_json Serialize() const;

        static void CreateOverrides(SavedWorldObject& object, const nlohmann::json& context, const std::string& currentObjectName);

        std::vector<SavedWorldObject> savedObjects;
    };

} // Vox
