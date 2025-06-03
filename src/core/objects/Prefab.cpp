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
    PrefabContext::PrefabContext(const std::string& filename)
    {
        using Json = nlohmann::json;

        const std::string& jsonString = ServiceLocator::GetFileIoService()->LoadFile("prefabs/" + filename);
        if (jsonString.empty())
        {
            return;
        }

        const Json prefabJson = Json::parse(jsonString);

        if (prefabJson.empty())
        {
            return;
        }

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

    PrefabContext::PrefabContext(const nlohmann::json& jsonObject)
    {
        if (!jsonObject.contains("class") || !jsonObject["class"].is_string())
        {
            VoxLog(Warning, Game, "Unable to parse object from json. The object does not have a valid class. "
                "Check that the json is not malformed or corrupted.");
            return;
        }

        const ObjectClass* baseClass = ServiceLocator::GetObjectService()->GetObjectClass(jsonObject["class"]);
        if (!baseClass)
        {
            VoxLog(Warning, Game, "Prefab could not find class '{}'.", jsonObject["class"].get<std::string>());
            return;
        }

        parent = baseClass;
        CreateOverrides(jsonObject, {});
    }

    PrefabContext::PrefabContext(const Object* object)
    {
        parent = object->GetClass();
        propertyOverrides = object->GenerateOverrides();
    }

    void PrefabContext::CreateOverrides(const nlohmann::json& context, const std::vector<std::string>& currentPathStack)
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
                propertyOverrides.emplace_back(currentPathStack, property.key(), propertyParsed);
            }
        }

        if (context.contains("children") && context["children"].is_object())
        {
            const auto& childrenObject = context["children"];
            for (const auto& child : childrenObject.items())
            {
                std::vector<std::string> childPathStack = currentPathStack;
                childPathStack.emplace_back(child.key());
                CreateOverrides(child.value(), childPathStack);
            }
        }
    }

    Prefab::Prefab(const std::string& filename)
        :ObjectClass({}, {}, {})
    {
        canBeRenamed = true;

        auto context = std::make_shared<PrefabContext>(filename);
        parentClass = context->parent;
        constructor = [context](const ObjectInitializer& objectInitializer)
        {
            return Construct(objectInitializer, context.get());
        };
    }

    Prefab::Prefab(const nlohmann::json& jsonObject)
        :ObjectClass({}, {}, {})
    {
        canBeRenamed = true;

        auto context = std::make_shared<PrefabContext>(jsonObject);
        parentClass = context->parent;
        constructor = [context](const ObjectInitializer& objectInitializer)
        {
            return Construct(objectInitializer, context.get());
        };
    }

    Prefab::Prefab(const Object* object)
        :ObjectClass({}, {}, {})
    {
        canBeRenamed = true;

        auto context = std::make_shared<PrefabContext>(object);
        parentClass = context->parent;
        constructor = [context](const ObjectInitializer& objectInitializer)
        {
            return Construct(objectInitializer, context.get());
        };
    }

    std::shared_ptr<Object> Prefab::Construct(const ObjectInitializer& objectInitializer, const PrefabContext* prefabContext)
    {
        if (!prefabContext->parent)
        {
            return {};
        }

        std::shared_ptr<Object> object = prefabContext->parent->GetConstructor()(objectInitializer);
        for (const auto& override : prefabContext->propertyOverrides)
        {
            OverrideProperty(object, override);
        }

        return object;
    }

    void Prefab::OverrideProperty(const std::shared_ptr<Object>& object, const PropertyOverride& propertyOverride)
    {
        std::shared_ptr<Object> currentObject = object;
        for (const auto& objectName : propertyOverride.path)
        {
            currentObject = currentObject->GetChildByName(objectName);
            if (!currentObject)
            {
                VoxLog(Warning, Game, "Prefab could not override property. Object '{}' was not found.", objectName);
                return;
            }
        }

        Property* property = currentObject->GetClass()->GetPropertyByName(propertyOverride.propertyName);
        if (!property)
        {
            VoxLog(Warning, Game, "Prefab could not override property. Property '{}' was not a member of '{}'.", propertyOverride.propertyName, currentObject->GetDisplayName());
            return;
        }

        property->SetValue(currentObject.get(), propertyOverride.variant.type, propertyOverride.variant.value);
        currentObject->PropertyChanged(*property);
    }
} // Vox