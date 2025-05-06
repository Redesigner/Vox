#pragma once
#include "core/objects/Object.h"
#include "core/objects/component/Component.h"

namespace  Vox
{
    class Component;

    class Actor : public Object
    {
    public:
        void BuildProperties(std::vector<Property>& propertiesInOut) override;

        const std::vector<std::unique_ptr<Component>>& GetComponents() const;
        
    protected:
        void RegisterComponent(std::unique_ptr<Component> component);
        
    private:
        Transform transform;

        std::vector<std::unique_ptr<Component>> components;
        
        IMPLEMENT_OBJECT(Actor)
    };
}
