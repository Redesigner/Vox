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

        void Activate() const;

        void OnTransformUpdated() override;

    private:
        IMPLEMENT_OBJECT(CameraComponent)

        Ref<Camera> camera;
    };
}
    
