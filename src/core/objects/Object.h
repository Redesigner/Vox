#pragma once
#include <vector>

#include "core/objects/Property.h"

#define IMPLEMENT_PROPERTIES() inline static std::vector<Property> properties;\
public: std::vector<Property>& GetProperties() override { return properties; }\
private:

namespace Vox
{
    class Object
    {
    public:
        Object();
        virtual ~Object() = default;

        void InitProperties();

        virtual std::vector<Property>& GetProperties() = 0;

    protected:
        virtual void BuildProperties(std::vector<Property>& propertiesInOut) = 0;
    };
}
