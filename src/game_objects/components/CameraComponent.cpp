﻿#include "CameraComponent.h"

#include "core/logging/Logging.h"
#include "core/objects/World.h"
#include "core/objects/actor/Actor.h"
#include "core/services/ServiceLocator.h"
#include "physics/PhysicsServer.h"
#include "physics/TypeConversions.h"
#include "rendering/Camera.h"
#include "rendering/Renderer.h"
#include "rendering/SceneRenderer.h"

namespace Vox
{
    CameraComponent::CameraComponent(Actor* parent)
        :SceneComponent(parent)
    {
        DEFAULT_DISPLAY_NAME();

        if (parent && parent->GetWorld())
        {
            camera = parent->GetWorld()->GetRenderer()->CreateCamera();
        }
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
        GetParent()->GetWorld()->GetRenderer()->SetCurrentCamera(camera);
    }

    void CameraComponent::SetArmLength(float length)
    {
        armLength = length;
    }

    void CameraComponent::OnTransformUpdated()
    {
        // @TODO: this is just a temp fix
        if (!camera)
        {
            return;
        }
        const Transform& worldTransform = GetWorldTransform();
        camera->SetPosition(worldTransform.position);
        camera->SetRotation(-worldTransform.rotation);
    }

    void CameraComponent::Tick(float deltaTime)
    {
        // @TODO: this is just a temp fix
        if (!camera)
        {
            return;
        }
        RayCastResultNormal rayCastResult;
        const glm::vec3 start = GetWorldTransform().position;
        const glm::vec3 forward = GetWorldTransform().GetForwardVector();
        if (ServiceLocator::GetPhysicsServer()->RayCast(start, forward * armLength, rayCastResult))
        {
            camera->SetPosition(start + forward * armLength * rayCastResult.percentage * 0.95f);
        }
        else
        {
            camera->SetPosition(start + forward * armLength);
        }
    }
}
