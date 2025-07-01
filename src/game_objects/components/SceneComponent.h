#pragma once
#include "core/objects/component/Component.h"

namespace Vox
{
    class SceneComponent : public Component
    {
    public:
        explicit SceneComponent(const ObjectInitializer& objectInitializer);

        void PropertyChanged(const Property& property) override;
        
        void BuildProperties(std::vector<Property>& propertiesInOut) override;

        void UpdateParentTransform(const Transform& transform);

        void AttachComponent(const std::shared_ptr<SceneComponent>& attachment);
        
        void SetPosition(glm::vec3 position);
        void SetRotation(glm::vec3 rotation);
        void SetScale(glm::vec3 scale);
        void SetTransform(const Transform& transformIn);

        [[nodiscard]] glm::vec3 GetLocalPosition() const;
        [[nodiscard]] glm::quat GetLocalRotation() const;
        [[nodiscard]] glm::vec3 GetLocalScale() const;
        [[nodiscard]] const Transform& GetLocalTransform() const;

        [[nodiscard]] Transform GetWorldTransform() const;

        [[nodiscard]] World* GetWorld() const;

        [[nodiscard]] const Actor* GetRoot() const;

        [[nodiscard]] std::vector<std::string> GetRootPath() const;

        [[nodiscard]] std::shared_ptr<SceneComponent> GetChildByPath(const std::vector<std::string>& path) const;

        /**
         * @brief Lookup a child component by name
         * @param name Name to search for
         * @return shared_ptr. Can be nullptr if no child has this name
         */
        [[nodiscard]] std::shared_ptr<SceneComponent> GetAttachmentByName(const std::string& name) const;

        [[nodiscard]] SceneComponent* GetParentAttachment() const;

        void SetParentAttachment(SceneComponent* parent);

        [[nodiscard]] const std::vector<std::shared_ptr<SceneComponent>>& GetAttachments() const;

#ifdef EDITOR
        virtual void Select() {}
#endif

    protected:
        virtual void OnTransformUpdated() {}

    private:
        void UpdateTransform();

        std::vector<std::shared_ptr<SceneComponent>> attachedComponents;

        SceneComponent* parentAttachment;
        
        Transform worldTransform;
        
        Transform localTransform;

        IMPLEMENT_OBJECT(SceneComponent, Component)
    };
}
