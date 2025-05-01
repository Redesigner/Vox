#pragma once
#include <vector>

#include "core/objects/Property.h"

#define IMPLEMENT_PROPERTIES() inline static std::vector<Property> properties;\
    public: std::vector<Property>& GetProperties() const override { return properties; }\
    private:

#define IMPLEMENT_NAME(Name) inline static std::string displayName = #Name;\
    const std::string& GetDisplayName() const { return displayName; }

namespace Vox
{
    class Object
    {
    public:
        Object();
        virtual ~Object() = default;

        void InitProperties();

        virtual std::vector<Property>& GetProperties() const = 0;

        virtual const std::string& GetDisplayName() const = 0;

    protected:
        virtual void BuildProperties(std::vector<Property>& propertiesInOut) = 0;
    };
}
