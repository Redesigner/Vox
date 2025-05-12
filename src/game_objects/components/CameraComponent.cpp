#include "CameraComponent.h"

#include "core/services/ServiceLocator.h"
#include "rendering/Camera.h"
#include "rendering/Renderer.h"

namespace Vox
{
    CameraComponent::CameraComponent()
    {
        DEFAULT_DISPLAY_NAME();

        camera = ServiceLocator::GetRenderer()->CreateCamera();
    }

    void CameraComponent::Activate() const
    {
        ServiceLocator::GetRenderer()->SetCurrentCamera(camera);
    }

    void CameraComponent::OnTransformUpdated()
    {
        const Transform& worldTransform = GetWorldTransform();
        camera->SetPosition(worldTransform.position);
        camera->SetRotation(worldTransform.rotation);
    }
}
