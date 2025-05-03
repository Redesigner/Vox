#pragma once
#include <string>
#include <unordered_map>

#include "core/concepts/Concepts.h"
#include "core/objects/ObjectClass.h"

namespace Vox
{    
    class ObjectService
    {
    public:
        const ObjectClass* RegisterObjectClass(const std::string& classId, const ObjectClass& objectClass);
        
        template <typename T>
        void RegisterObjectClass() requires Derived<T, Object>
        {
            std::vector<Property> properties;
            T defaultObject = T();
            defaultObject.BuildProperties(properties);
            T::SetObjectClass(RegisterObjectClass(defaultObject.GetClassDisplayName(), ObjectClass(T::template GetConstructor<T>(), properties)));
        }

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

        const ObjectClass* GetObjectClass(const std::string& objectClassId) const;

    private:
        std::unordered_map<std::string, ObjectClass> classRegistry;
    };
}
