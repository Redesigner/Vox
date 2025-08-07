//
// Created by steph on 5/27/2025.
//

#include "Prefab.h"

#include <nlohmann/json.hpp>

#include "core/logging/Logging.h"
#include "../../../game_objects/actors/Actor.h"
#include "core/math/Strings.h"
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
            assert(false);
        }

        const std::shared_ptr<ObjectClass> baseClass = ServiceLocator::GetObjectService()->GetObjectClass(root["class"]);
        assert(baseClass && "Base class was not found.");

        className = jsonObject.items().begin().key();
        parentClass = baseClass;
        CreateOverrides(root, {});
        CreateAdditionalObjects(root);
    }

    PrefabContext::PrefabContext(const Object* object, const std::weak_ptr<ObjectClass>& baseClass)
    {
        className = object->GetClass()->GetName();
        parentClass = object->GetClass();
        assert(!parentClass.expired() && parentClass.lock());
        std::vector<std::string> context;
        const Object* defaultObject = baseClass.lock()->GetDefaultObject();
        propertyOverrides = object->GenerateOverrides(defaultObject);
        if (const auto* actor = dynamic_cast<const Actor*>(object))
        {
            const auto* defaultActor = dynamic_cast<const Actor*>(defaultObject);
            for (const auto& childObject : actor->GetChildren())
            {
                if (childObject->native)
                {
                    const std::vector<PropertyOverride> childOverrides = childObject->GenerateOverrides(
                        defaultActor->GetChildByName(childObject->GetDisplayName()).get()
                    );
                    propertyOverrides.insert(propertyOverrides.end(), childOverrides.begin(), childOverrides.end());
                }
                else
                {
                    const std::vector<PropertyOverride> childOverrides = childObject->GenerateOverrides(
                        childObject->GetClass()->GetDefaultObject()
                    );
                    propertyOverrides.insert(propertyOverrides.end(), childOverrides.begin(), childOverrides.end());
                    additionalObjects.emplace_back(childObject->GetClass(), childObject->GetDisplayName());
                }
            }
        }
    }

    void PrefabContext::CreateOverrides(const nlohmann::json& context, const std::string& childName) // NOLINT(*-no-recursion)
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
                std::vector path = {childName};
                propertyOverrides.emplace_back(childName, property.key(), propertyParsed);
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
        context->parentClass.lock()), context(context)
    {
        name = context->className;
        canBeRenamed = true;
    }

    Prefab::~Prefab()
    = default;

    std::unique_ptr<Prefab> Prefab::FromObject(const Object* object, std::weak_ptr<ObjectClass> baseClass)
    {
        if (!baseClass.lock())
        {
            baseClass = object->GetClass();
        }
        auto context = std::make_shared<PrefabContext>(object, baseClass);
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
        const auto updatedPrefabContext = PrefabContext(object, GetParentClass());
        context->additionalObjects = updatedPrefabContext.additionalObjects;
        context->propertyOverrides = updatedPrefabContext.propertyOverrides;

        ServiceLocator::GetObjectService()->UpdateClass(this);
        defaultObject = Construct(ObjectInitializer(), context.get());
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
            if (!propertyOverride.path.empty())
            {
                Json& propertyRootJson = *currentContext;
                currentContext = &propertyRootJson[propertyOverride.path.front()];
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

    std::shared_ptr<PrefabContext> Prefab::GetContext() const
    {
        return context;
    }

    std::shared_ptr<Object> Prefab::Construct(const ObjectInitializer& objectInitializer, const PrefabContext* prefabContext)
    {
        if (prefabContext->parentClass.expired())
        {
            return {};
        }

        std::shared_ptr<Object> object = prefabContext->parentClass.lock()->GetConstructor()(objectInitializer);
        object->localClass = ServiceLocator::GetObjectService()->GetObjectClass(prefabContext->className);

        ObjectInitializer childInitializer = objectInitializer;
        childInitializer.rootObject = false;

        // @TODO: Restrict adding children to actor prefabs only
        if (const auto actor = std::dynamic_pointer_cast<Actor>(object))
        {
            childInitializer.parent = actor.get();

            for (auto& child : actor->GetChildren())
            {
                child->native = true;
            }

            for (const auto& [objectClass, objectName] : prefabContext->additionalObjects)
            {
                std::shared_ptr<ObjectClass> lockedClass = objectClass.lock();
                if (lockedClass->IsA<SceneComponent>())
                {
                    const std::shared_ptr<SceneComponent> child = actor->AttachComponent(lockedClass.get());
                    child->SetName(objectName);
                    child->native = false;
                }
                else
                {
                    std::shared_ptr<Component> child = std::dynamic_pointer_cast<Component>(objectClass.lock()->GetConstructor()(childInitializer));
                    child->SetName(objectName);
                    child->native = false;
                    actor->AddChild(child);
                }
            }
        }

        for (const auto& override : prefabContext->propertyOverrides)
        {
            OverrideProperty(object, override);
        }

        if (objectInitializer.rootObject)
        {
            object->PostConstruct();
        }

        return object;
    }

    void Prefab::OverrideProperty(const std::shared_ptr<Object>& object, const PropertyOverride& propertyOverride)
    {
        std::shared_ptr<Object> currentObject = object;

        if (const auto& sceneComponent = std::dynamic_pointer_cast<Actor>(object))
        {
            currentObject = sceneComponent->GetChildByName(propertyOverride.path);
            if (!currentObject)
            {
                return;
            }
        }

        const Property* property = currentObject->GetClass()->GetPropertyByName(propertyOverride.propertyName);
        if (!property)
        {
            VoxLog(Warning, Game, "Prefab could not override property. Property '{}':'{}' was not a member of '{}'.", propertyOverride.path, propertyOverride.propertyName, currentObject->GetClassDisplayName());
            return;
        }

        property->SetValue(currentObject.get(), propertyOverride.variant.type, propertyOverride.variant.value);
        currentObject->PropertyChanged(*property);
    }
} // Vox