#pragma once
#include "SceneComponent.h"
#include "core/datatypes/Ref.h"
#include "core/objects/Tickable.h"
#include "rendering/Camera.h"

namespace Vox
{
    class CameraComponent : public SceneComponent, public Tickable
    {
    public:
        CameraComponent();

        void BuildProperties(std::vector<Property>& propertiesInOut) override;

        void OnTransformUpdated() override;

        void Tick(float deltaTime);

        void Activate() const;

        void SetArmLength(float length);

    private:
        IMPLEMENT_OBJECT(CameraComponent)

        float armLength = 5.0f;

        Ref<Camera> camera;
    };
}
    
