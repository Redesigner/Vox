#pragma once
#include "SceneComponent.h"
#include "core/objects/Tickable.h"
#include "../../rendering/camera/Camera.h"

namespace Vox
{
    class CameraComponent : public SceneComponent, public Tickable
    {
    public:
        CameraComponent(const ObjectInitializer& objectInitializer);

        void BuildProperties(std::vector<Property>& propertiesInOut) override;

        void OnTransformUpdated() override;

        void Tick(float deltaTime) override;

        void Activate() const;

        void SetArmLength(float length);

    private:
        IMPLEMENT_OBJECT(CameraComponent, SceneComponent)

        float armLength = 5.0f;

        std::shared_ptr<Camera> camera;

        World* owningWorld;
    };
}
    
