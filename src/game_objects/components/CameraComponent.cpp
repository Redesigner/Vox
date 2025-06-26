#include "CameraComponent.h"

#include "core/logging/Logging.h"
#include "core/objects/world/World.h"
#include "core/objects/actor/Actor.h"
#include "physics/PhysicsServer.h"
#include "physics/TypeConversions.h"
#include "rendering/camera/Camera.h"
#include "rendering/Renderer.h"
#include "rendering/SceneRenderer.h"

namespace Vox
{
    CameraComponent::CameraComponent(const ObjectInitializer& objectInitializer)
        :SceneComponent(objectInitializer)
    {
        DEFAULT_DISPLAY_NAME();

        camera = std::make_shared<Camera>();
        camera->SetPosition(0.0f, 0.0f, 0.0f);
        camera->SetRotation(0.0f, 0.0f, 0.0f);
        camera->SetFovY(45.0f);

        owningWorld = objectInitializer.world;
    }

    void CameraComponent::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        SceneComponent::BuildProperties(propertiesInOut);
        REGISTER_PROPERTY(float, armLength);
    }

    void CameraComponent::Activate() const
    {
        if (!camera)
        {
            return;
        }
        if (owningWorld)
        {
            owningWorld->GetRenderer()->SetCurrentCamera(camera);
        }
    }

    void CameraComponent::SetArmLength(float length)
    {
        armLength = length;
    }

    void CameraComponent::OnTransformUpdated()
    {
        // The camera's position here is updated by the raycast in tick
        const Transform& worldTransform = GetWorldTransform();
        camera->SetRotation(-worldTransform.rotation);
    }

    void CameraComponent::Tick(float deltaTime)
    {
        RayCastResultNormal rayCastResult;
        const glm::vec3 start = GetWorldTransform().position;
        const glm::vec3 forward = GetWorldTransform().GetForwardVector();
        if (owningWorld && owningWorld->GetPhysicsServer()->RayCast(start, forward * armLength, rayCastResult))
        {
            camera->SetPosition(start + forward * armLength * rayCastResult.percentage * 0.95f);
        }
        else
        {
            camera->SetPosition(start + forward * armLength);
        }
    }
}
