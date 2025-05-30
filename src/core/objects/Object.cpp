#include "Object.h"

#include <nlohmann/json.hpp>

#include "ObjectClass.h"
#include "core/math/Strings.h"

namespace Vox
{
    ObjectInitializer::ObjectInitializer()
    = default;

    ObjectInitializer::ObjectInitializer(World* world)
        :world(world)
    {
    }

    ObjectInitializer::ObjectInitializer(Object* parent)
        :parent(parent)
    {
    }

    Object::Object(const ObjectInitializer& objectInitializer)
        :parent(objectInitializer.parent)
    {
    }

    const std::vector<Property>& Object::GetProperties() const
    {
        const ObjectClass* objectClass = GetClass();
        assert(objectClass && "ObjectClass was invalid");

        // ReSharper disable once CppDFANullDereference
        return objectClass->GetProperties();
    }

    const std::string& Object::GetDisplayName() const
    {
        return displayName;
    }

    void Object::SetName(const std::string& name)
    {
        displayName = name;
    }

    void Object::AddChild(const std::shared_ptr<Object>& child)
    {
        for (const auto& existingChild : children)
        {
            // check name collisions
            if (existingChild->GetDisplayName() == child->GetDisplayName())
            {
                child->SetName(IncrementString(child->GetDisplayName()));
            }
        }
        children.emplace_back(child);
    }

    const std::vector<std::shared_ptr<Object>>& Object::GetChildren() const
    {
        return children;
    }

    // May need to create some kind of stack limit here
    nlohmann::ordered_json Object::Serialize(std::shared_ptr<Object> defaultObject) // NOLINT(*-no-recursion)
    {
        using json = nlohmann::ordered_json;

        if (!defaultObject)
        {
            defaultObject = GetClass()->GetConstructor()(ObjectInitializer());
        }

        json objectJson {};
        objectJson[displayName]["class"] = GetClassDisplayName();

        objectJson[displayName]["properties"] = json::value_type::object();
        for (const Property& property : GetProperties())
        {
            if (property.ValueEquals(defaultObject.get(), this))
            {
                continue;
            }

            json propertyJson = property.Serialize(this);
            objectJson[displayName]["properties"].insert(propertyJson.begin(), propertyJson.end());
        }

        if (!children.empty())
        {
            objectJson[displayName]["children"] = json::value_type::object();
            for (const auto& child : children)
            {
                // Match children by names
                json childJson = child->Serialize(defaultObject->GetChildByName(child->GetDisplayName()));
                objectJson[displayName]["children"].insert(childJson.begin(), childJson.end());
            }
        }


        if (objectJson[displayName]["children"].empty() && objectJson[displayName]["properties"].empty())
        {
            objectJson.erase(displayName);
            return objectJson;
        }

        if (objectJson[displayName]["properties"].empty())
        {
            objectJson[displayName].erase("properties");
        }

        if (objectJson[displayName]["children"].empty())
        {
            objectJson[displayName].erase("children");
        }
        return objectJson;
    }

    std::vector<PropertyOverride> Object::GenerateOverrides() const
    {
        std::vector<std::string> context {};
        return GenerateOverrides(GetClass()->GetConstructor()(ObjectInitializer()), context);
    }

    std::vector<PropertyOverride> Object::GenerateOverrides(std::shared_ptr<Object> defaultObject,
        std::vector<std::string>& context) const
    {
        std::vector<PropertyOverride> result;
        for (const Property& property : GetProperties())
        {
            if (property.ValueEquals(defaultObject.get(), this))
            {
                continue;
            }

            result.emplace_back(context, property.GetName(), property.GetTypedVariant(this));
        }

        if (!children.empty())
        {
            for (const auto& child : children)
            {
                std::vector<std::string> childContext = context;
                childContext.emplace_back(child->GetDisplayName());
                std::vector<PropertyOverride> childOverrides = child->GenerateOverrides(
                    defaultObject->GetChildByName(child->GetDisplayName()), childContext
                );
                result.insert(result.end(), childOverrides.begin(), childOverrides.end());
            }
        }

        return result;
    }

    Object* Object::GetParent() const
    {
        return parent;
    }

    std::shared_ptr<Object> Object::GetSharedThis() const
    {
        if (!parent)
        {
            return {};
        }

        if (auto result = parent->GetSharedChild(this))
        {
            return result;
        }

        assert(true && "Child object may be orphaned");
        return {};
    }

    std::shared_ptr<Object> Object::GetSharedChild(const Object* object) const
    {
        auto result = std::ranges::find_if(children, [object](const std::shared_ptr<Object>& childObject)
        {
            return childObject.get() == object;
        });

        if (result != children.end())
        {
            return *result;
        }

        return {};
    }

    std::shared_ptr<Object> Object::GetChildByName(const std::string& name) const
    {
        for (const auto& child : children)
        {
            if (child->GetDisplayName() == name)
            {
                return child;
            }
        }

        return {};
    }
}
