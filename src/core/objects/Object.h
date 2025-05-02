#pragma once
#include <functional>
#include <regex>
#include <vector>
#include <fmt/format.h>

#include "core/concepts/Concepts.h"
#include "core/objects/Property.h"

#define IMPLEMENT_PROPERTIES() bool testReplacement = false;

#define IMPLEMENT_NAME(Name) inline static std::string classDisplayName = std::regex_replace(std::string(#Name), std::regex("(\\B[A-Z])"), " $1");\
    public: std::string& GetClassDisplayName() const override { return classDisplayName; }\
    private:

#define DEFAULT_DISPLAY_NAME() displayName = fmt::format("{}_Default", classDisplayName);

namespace Vox
{
    class ObjectClass;

    /// Object is an object type that has reflected properties
    class Object
    {
    public:
        Object() = default;
        virtual ~Object() = default;

        Object(Object&) = delete;
        Object(Object&&) = delete;
        void operator =(Object&) = delete;
        Object& operator =(Object&&) = delete;
        
        template <typename T>
        static std::function<T*()> GetConstructor() requires Derived<T, Object>
        {
            return [] { return new T; };
        }

        const std::vector<Property>& GetProperties() const;

        const ObjectClass* GetClass() const;

        virtual std::string& GetClassDisplayName() const = 0;

        const std::string& GetDisplayName() const;

        virtual void BuildProperties(std::vector<Property>& propertiesInOut) = 0;
        
    protected:
        std::string displayName;
    };
}
