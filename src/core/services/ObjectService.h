#pragma once
#include <string>
#include <unordered_map>

#include "core/concepts/Concepts.h"
#include "core/datatypes/Delegate.h"
#include "core/objects/Object.h"
#include "core/objects/ObjectClass.h"
#include "../objects/prefabs/Prefab.h"

namespace Vox
{    
    class ObjectService
    {
    public:
        using ObjectMap = std::map<std::string, std::shared_ptr<ObjectClass>>;

        ObjectService() = default;
        ~ObjectService() = default;

        template <typename T>
        void RegisterObjectClass() requires Derived<T, Object>
        {
            auto objectClass = std::make_shared<ObjectClass>(T::template GetConstructor<T>(), T::GetParentClass());
            classRegistry.emplace(objectClass->GetName(), objectClass);
            T::SetStaticObjectClass(objectClass);
        }

        void RegisterPrefab(const std::string& filename);

        /// Register an object class, skipping if the class is already registered
        template <typename T>
        void RegisterObjectClassConditional() requires Derived<T, Object>
        {
            if (classRegistry.contains(T().GetClassDisplayName()))
            {
                return;
            }
            RegisterObjectClass<T>();
        }

        [[nodiscard]] std::shared_ptr<ObjectClass> GetObjectClass(const std::string& objectClassId) const;


        [[nodiscard]] ObjectMap::const_iterator GetBegin() const;
        [[nodiscard]] ObjectMap::const_iterator GetEnd() const;

        [[nodiscard]] DelegateHandle<const ObjectClass*> RegisterClassChangedDelegate(const std::function<void(const ObjectClass*)>& delegate);
        void UnregisterClassChangedDelegate(DelegateHandle<const ObjectClass*> handle);

        void UpdateClass(const ObjectClass* objectClass);

    private:
        ObjectMap classRegistry;

        Delegate<const ObjectClass*> objectClassChanged;
    };
}
