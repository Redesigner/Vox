//
// Created by steph on 5/27/2025.
//

#include "Prefab.h"

#include <nlohmann/json.hpp>

#include "core/logging/Logging.h"
#include "core/services/FileIOService.h"
#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    Prefab::Prefab(const std::string& filename)
    {
        using Json = nlohmann::json;

        const std::string& jsonString = ServiceLocator::GetFileIoService()->LoadFile("prefabs/" + filename);
        if (jsonString.empty())
        {
            return;
        }

        const Json prefabJson = Json::parse(jsonString);

        const Json& objectRoot = prefabJson.front();

        if (!objectRoot.contains("class") || !objectRoot["class"].is_string())
        {
            VoxLog(Warning, Game, "Unable to parse object from json. The object does not have a valid class. "
                "Check that the json is not malformed or corrupted.");
            return;
        }

        const ObjectClass* baseClass = ServiceLocator::GetObjectService()->GetObjectClass(objectRoot["class"]);
        if (!baseClass)
        {
            VoxLog(Warning, Game, "Prefab could not find class '{}'.", objectRoot["class"].get<std::string>());
            return;
        }

        parent = baseClass;
        CreateOverrides(objectRoot, {});
    }

    std::shared_ptr<Object> Prefab::Construct()
    {
        if (!parent)
        {
            return {};
        }

        std::shared_ptr<Object> object = parent->GetConstructor()(ObjectInitializer());

    }

    void Prefab::CreateOverrides(const nlohmann::json& context, std::vector<std::string> currentPathStack)
    {
        if (!context.contains("properties") || !context.is_object())
        {
            VoxLog(Warning, Game, "Unable to parse object from json. The object does not have a valid properties list. "
                "Check that the json is not malformed or corrupted.");
            return;
        }

        const auto& propertiesObject = context["properties"];
        for (auto& property : propertiesObject)
        {
            auto propertyParsed = Property::Deserialize(property);
            if (propertyParsed.first == PropertyType::_invalid)
            {
                break;
            }

            propertyOverrides.emplace_back(currentPathStack, propertyParsed.first, propertyParsed.second);
        }

        if (!context.contains("children"))
        {
            return;
        }

        if (!context["children"].is_object())
        {
            VoxLog(Warning, Game, "Error parsing prefab. A list of children was found, but it was invalid.");
            return;
        }

        const auto& childrenObject = context["children"];
        for (const auto& child : childrenObject.items())
        {
            std::vector<std::string> childPathStack = currentPathStack;
            childPathStack.emplace_back(child.key());
            CreateOverrides(child.value(), std::move(childPathStack));
        }
    }
} // Vox