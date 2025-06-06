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
    PrefabContext::PrefabContext(const nlohmann::json& jsonObject)
    {
        nlohmann::json root = jsonObject.front();
        if (!root.contains("class") || !root["class"].is_string())
        {
            VoxLog(Warning, Game, "Unable to parse object from json. The object does not have a valid class. "
                "Check that the json is not malformed or corrupted.");
            return;
        }

        const std::shared_ptr<ObjectClass> baseClass = ServiceLocator::GetObjectService()->GetObjectClass(root["class"]);
        if (!baseClass)
        {
            VoxLog(Warning, Game, "Prefab could not find class '{}'.", root["class"].get<std::string>());
            return;
        }

        className = jsonObject.items().begin().key();
        parent = baseClass;
        CreateOverrides(root, {});
        CreateAdditionalObjects(root);
    }

    PrefabContext::PrefabContext(const Object* object)
    {
        className = object->GetClass()->GetName();
        parent = object->GetClass();
        propertyOverrides = object->GenerateOverrides();

        for (const auto& childObject : object->GetChildren())
        {
            if (!childObject->native)
            {
                additionalObjects.emplace_back(childObject->GetClass(), childObject->GetDisplayName());
            }
        }
    }

    void PrefabContext::CreateOverrides(const nlohmann::json& context, const std::vector<std::string>& currentPathStack) // NOLINT(*-no-recursion)
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

    void PrefabContext::CreateAdditionalObjects(const nlohmann::json& context)
    {
        if (context.contains("additional") && context["additional"].is_object())
        {
            for (const auto& additionalChild : context["additional"].items())
            {
                if (!additionalChild.value().contains("class") || !additionalChild.value()["class"].is_string())
                {
                    continue;
                }

                if (auto childClass = ServiceLocator::GetObjectService()->GetObjectClass(additionalChild.value()["class"]))
                {
                    additionalObjects.emplace_back(childClass, additionalChild.key());
                }
            }
        }
    }

    Prefab::Prefab(const std::shared_ptr<PrefabContext>& context)
        :ObjectClass([context](const ObjectInitializer& objectInitializer) { return Construct(objectInitializer, context.get());},
        context->parent.lock()), context(context)
    {
        name = context->className;
        canBeRenamed = true;
    }

    Prefab::~Prefab()
    = default;

    std::unique_ptr<Prefab> Prefab::FromObject(const Object* object)
    {
        auto context = std::make_shared<PrefabContext>(object);
        return std::make_unique<Prefab>(context);
    }

    std::unique_ptr<Prefab> Prefab::FromJson(const nlohmann::json& json)
    {
        auto context = std::make_shared<PrefabContext>(json);
        return std::make_unique<Prefab>(context);
    }

    std::unique_ptr<Prefab> Prefab::FromFile(const std::string& filename)
    {
        using Json = nlohmann::json;

        const std::string& jsonString = ServiceLocator::GetFileIoService()->LoadFile("prefabs/" + filename);
        if (jsonString.empty())
        {
            return nullptr;
        }

        const Json prefabJson = Json::parse(jsonString);
        return FromJson(prefabJson);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void Prefab::SaveChanges(const Object* object)
    {
        const auto updatedPrefabContext = PrefabContext(object);
        context->additionalObjects = updatedPrefabContext.additionalObjects;
        context->propertyOverrides = updatedPrefabContext.propertyOverrides;

        ServiceLocator::GetObjectService()->UpdateClass(this);
    }

    void Prefab::SaveToFile(const std::string& filename) const
    {
        ServiceLocator::GetFileIoService()->WriteToFile("prefabs/" + filename, Serialize().dump(4));
    }

    nlohmann::ordered_json Prefab::Serialize() const
    {
        using Json = nlohmann::ordered_json;

        Json result = Json::object();
        result[GetName()]["class"] = GetParentClass()->GetName();
        for (const PropertyOverride& propertyOverride : context->propertyOverrides)
        {
            Json* currentContext = &result[GetName()]["children"];
            for (const std::string& path : propertyOverride.path)
            {
                Json& propertyRootJson = *currentContext;
                currentContext = &propertyRootJson[path];
            }
            const Json& propertyValueJson = propertyOverride.variant.Serialize();
            Json& propertyRootJson = *currentContext;
            propertyRootJson["properties"][propertyOverride.propertyName] = Json::object();
            propertyRootJson["properties"][propertyOverride.propertyName].insert(propertyValueJson.begin(), propertyValueJson.end());
        }

        result[GetName()]["additional"] = Json::object();
        for (const AdditionalObject& additionalObject : context->additionalObjects)
        {
            Json newObjectJson = Json::object();
            newObjectJson[additionalObject.objectName]["class"] = additionalObject.objectClass.lock()->GetName();
            result[GetName()]["additional"].insert(newObjectJson.begin(), newObjectJson.end());
        }
        return result;
    }

    std::shared_ptr<Object> Prefab::Construct(const ObjectInitializer& objectInitializer, const PrefabContext* prefabContext)
    {
        if (prefabContext->parent.expired())
        {
            return {};
        }

        std::shared_ptr<Object> object = prefabContext->parent.lock()->GetConstructor()(objectInitializer);
        object->localClass = ServiceLocator::GetObjectService()->GetObjectClass(prefabContext->className);

        ObjectInitializer childInitializer = objectInitializer;
        childInitializer.parent = object.get();
        for (const auto& additionalObject : prefabContext->additionalObjects)
        {
            std::shared_ptr<Object> child = additionalObject.objectClass.lock()->GetConstructor()(childInitializer);
            child->SetName(additionalObject.objectName);
            child->native = false;
            object->AddChild(child);
        }

        for (const auto& override : prefabContext->propertyOverrides)
        {
            OverrideProperty(object, override);
        }

        // object->SetClass

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