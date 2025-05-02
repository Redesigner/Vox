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
        void RegisterObjectClass(const std::string& classId, const ObjectClass& objectClass);
        
        
        template <typename T>
        void RegisterObjectClass() requires Derived<T, Object>
        {
            std::vector<Property> properties;
            T defaultObject = T();
            defaultObject.BuildProperties(properties);
            RegisterObjectClass(defaultObject.GetClassDisplayName(), ObjectClass(T::template GetConstructor<T>(), properties));
        }

        const ObjectClass* GetObjectClass(const std::string& objectClassId) const;

    private:
        std::unordered_map<std::string, ObjectClass> classRegistry;
    };
}
