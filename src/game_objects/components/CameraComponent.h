#pragma once
#include "SceneComponent.h"
#include "core/datatypes/Ref.h"

namespace Vox
{
    class Camera;

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

        float armLength = 1.0f;

        Ref<Camera> camera;
    };
}
    
