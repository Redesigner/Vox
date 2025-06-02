//
// Created by steph on 5/30/2025.
//

#include "SavedWorld.h"

#include <nlohmann/json.hpp>

namespace Vox
{
    SavedWorld::SavedWorld(const std::string& jsonString)
    {
        using Json = nlohmann::json;
        const Json worldJson = Json::parse(jsonString);
        if (!worldJson.contains("objects") || !worldJson["objects"].is_object())
        {
            return;
        }

        for (const auto& object : worldJson["objects"].items())
        {
            savedObjects.emplace_back(object.key(), std::move(Prefab(object.value())));
        }
    }
} // Vox