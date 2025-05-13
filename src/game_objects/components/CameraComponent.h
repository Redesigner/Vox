#pragma once
#include "SceneComponent.h"
#include "core/datatypes/Ref.h"
#include "rendering/Camera.h"

namespace Vox
{
    class CameraComponent : public SceneComponent
    {
    public:
        CameraComponent();

        void BuildProperties(std::vector<Property>& propertiesInOut) override;

        void OnTransformUpdated() override;

        void Update();

        void Activate() const;

        void SetArmLength(float length);

    private:
        IMPLEMENT_OBJECT(CameraComponent)

        float armLength = 5.0f;

        Ref<Camera> camera;
    };
}
    
