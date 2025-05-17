#pragma once
#include "core/objects/component/Component.h"

namespace Vox
{
    class SceneComponent : public Component
    {
    public:
        void PropertyChanged(const Property& property) override;
        
        void BuildProperties(std::vector<Property>& propertiesInOut) override;

        void UpdateParentTransform(const Transform& transform);
        
        void SetPosition(glm::vec3 position);
        void SetRotation(glm::vec3 rotation);
        void SetScale(glm::vec3 scale);
        void SetTransform(const Transform& transformIn);

        [[nodiscard]] glm::vec3 GetLocalPosition() const;
        [[nodiscard]] glm::quat GetLocalRotation() const;
        [[nodiscard]] glm::vec3 GetLocalScale() const;
        [[nodiscard]] Transform GetLocalTransform() const;

        [[nodiscard]] Transform GetWorldTransform() const;

#ifdef EDITOR
        virtual void Select() {}
#endif

    protected:
        virtual void OnTransformUpdated() {}

    private:
        void UpdateTransform();
        
        Transform worldTransform;
        
        Transform localTransform;

        IMPLEMENT_OBJECT(SceneComponent)
    };
}
