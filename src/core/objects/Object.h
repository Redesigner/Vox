#pragma once
#include <regex>
#include <vector>
#include <fmt/format.h>

#include "core/objects/Property.h"

#define IMPLEMENT_PROPERTIES() inline static std::vector<Property> properties;\
    public: std::vector<Property>& GetProperties() const override { return properties; }\
    private:

#define IMPLEMENT_NAME(Name) inline static std::string classDisplayName = std::regex_replace(std::string(#Name), std::regex("(\\B[A-Z])"), " $1");\
    const std::string& GetClassDisplayName() const { return classDisplayName; }

namespace Vox
{
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
        

        void InitProperties();

        virtual std::vector<Property>& GetProperties() const = 0;

        virtual const std::string& GetClassDisplayName() const = 0;

        const std::string& GetDisplayName() const;

    protected:
        virtual void BuildProperties(std::vector<Property>& propertiesInOut) = 0;

        std::string displayName;
    };
}
