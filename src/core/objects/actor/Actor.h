#pragma once
#include "core/objects/Object.h"
#include "core/objects/component/Component.h"
#include "components/SceneComponent.h"


namespace  Vox
{
    class Component;

    class Actor : public Object
    {
    public:
        void BuildProperties(std::vector<Property>& propertiesInOut) override;

        void PropertyChanged(const Property& property) override;

        [[nodiscard]] const std::vector<std::unique_ptr<Component>>& GetComponents() const;
        
        [[nodiscard]] const std::vector<std::unique_ptr<SceneComponent>>& GetAttachedComponents() const;

        void SetPosition(glm::vec3 position);
        void SetRotation(glm::quat rotation);
        void SetScale(glm::vec3 scale);
        void SetTransform(const Transform& transformIn);

        [[nodiscard]] const Transform& GetTransform() const;
        
    protected:
        void RegisterComponent(std::unique_ptr<Component> component);

        void AttachComponent(std::unique_ptr<SceneComponent> component);
    
    private:
        void UpdateChildTransforms() const;
        
        Transform transform;

        std::vector<std::unique_ptr<Component>> components;

        std::vector<std::unique_ptr<SceneComponent>> attachedComponents;
        
        IMPLEMENT_OBJECT(Actor)
    };
}
