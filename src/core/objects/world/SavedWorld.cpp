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
            assert(false);
            return;
        }

        for (const auto& object : worldJson["objects"].items())
        {
            auto& newObject = savedObjects.emplace_back();
            newObject.name = object.key();
            newObject.className = object.value()["class"];
            CreateOverrides(newObject, object.value(), {});
        }
    }

    nlohmann::ordered_json SavedWorld::Serialize() const
    {
        using Json = nlohmann::ordered_json;

        Json result = Json::object();
        result["objects"] = Json::object();
        for (const auto& savedObject : savedObjects)
        {
            result["objects"][savedObject.name]["class"] = savedObject.className;

            if (savedObject.worldContextOverrides.empty())
            {
                continue;
            }


            result["objects"][savedObject.name]["properties"] = Json::object();
            for (const auto& propertyOverride : savedObject.worldContextOverrides)
            {
                Json* currentContext = propertyOverride.path.empty() ? &result["objects"][savedObject.name] : &result["objects"][savedObject.name]["children"][propertyOverride.path];
                const Json& propertyValueJson = propertyOverride.variant.Serialize();
                Json& propertyRootJson = *currentContext;
                propertyRootJson["properties"][propertyOverride.propertyName] = Json::object();
                propertyRootJson["properties"][propertyOverride.propertyName].insert(propertyValueJson.begin(), propertyValueJson.end());
            }
        }
        return result;
    }

    void SavedWorld::CreateOverrides(SavedWorldObject& object, const nlohmann::json& context, const std::string& currentObjectName) // NOLINT(*-no-recursion)
    {
        if (context.contains("properties") && context["properties"].is_object())
        {
            const auto& propertiesObject = context["properties"];
            for (auto& property : propertiesObject.items())
            {
                auto propertyParsed = Property::Deserialize(property.value());
                if (propertyParsed.type == PropertyType::_invalid)
                {
                    break;
                }
                object.worldContextOverrides.emplace_back(currentObjectName, property.key(), propertyParsed);
            }
        }

        if (context.contains("children") && context["children"].is_object())
        {
            const auto& childrenObject = context["children"];
            for (const auto& child : childrenObject.items())
            {
                CreateOverrides(object, child.value(), child.key());
            }
        }
    }
} // Vox