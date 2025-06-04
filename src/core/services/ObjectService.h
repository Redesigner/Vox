﻿#pragma once
#include <string>
#include <unordered_map>

#include "core/concepts/Concepts.h"
#include "core/objects/Object.h"
#include "core/objects/ObjectClass.h"
#include "core/objects/Prefab.h"

namespace Vox
{    
    class ObjectService
    {
    public:
        using ObjectMap = std::map<std::string, std::shared_ptr<ObjectClass>>;

        std::shared_ptr<ObjectClass> RegisterObjectClass(const std::string& classId, const std::shared_ptr<ObjectClass>& objectClass);
        
        template <typename T>
        void RegisterObjectClass() requires Derived<T, Object>
        {
            std::vector<Property> properties;
            T defaultObject = T(ObjectInitializer());
            defaultObject.BuildProperties(properties);
            T::SetObjectClass(
                RegisterObjectClass(defaultObject.GetClassDisplayName(),
                    std::make_shared<ObjectClass>(
                        defaultObject.GetClassDisplayName(),
                        T::template GetConstructor<T>(),
                        T::GetParentClass(),
                        properties)
                )
            );
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

    private:
        ObjectMap classRegistry;
    };
}
