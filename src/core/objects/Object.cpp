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

    nlohmann::ordered_json Object::Serialize()
    {
        using json = nlohmann::ordered_json;

        json objectJson {};
        objectJson[displayName]["class"] = GetClassDisplayName();

        for (const Property& property : GetProperties())
        {
            json propertyJson = property.Serialize(this);
            objectJson[displayName].insert(propertyJson.begin(), propertyJson.end());
        }
        return objectJson;
    }

    Object* Object::GetParent() const
    {
        return parent;
    }

    std::shared_ptr<Object> Object::GetSharedChild() const
    {
        for (const std::shared_ptr<Object>& child : parent->GetChildren())
        {
            if (child.get() == this)
            {
                return child;
            }
        }

        return {};
    }

    std::shared_ptr<Object> Object::GetSharedThis() const
    {
        if (!parent)
        {
            return {};
        }

        if (auto result = parent->GetSharedChild())
        {
            return result;
        }

        assert(true && "Child object may be orphaned");
        return {};
    }

    std::shared_ptr<Object> Object::GetSharedChild(Object* object) const
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
}
