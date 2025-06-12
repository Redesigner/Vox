//
// Created by steph on 6/10/2025.
//

#include "CharacterPhysicsComponent.h"

#include "core/objects/World.h"
#include "core/objects/actor/Actor.h"
#include "physics/PhysicsServer.h"
#include "physics/TypeConversions.h"

namespace Vox
{
    CharacterPhysicsComponent::CharacterPhysicsComponent(const ObjectInitializer& objectInitializer)
        :SceneComponent(objectInitializer)
    {
        radius = 0.0f;
        halfHeight = 0.0f;
    }

    CharacterPhysicsComponent::CharacterPhysicsComponent(const ObjectInitializer& objectInitializer, float radius, float halfHeight)
        :CharacterPhysicsComponent(objectInitializer)
    {
        this->radius = radius;
        this->halfHeight = halfHeight;
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void CharacterPhysicsComponent::AddImpulse(const glm::vec3 impulse)
    {
        if (characterController)
        {
            characterController->AddImpulse(Vec3From(impulse));
        }
    }

    bool CharacterPhysicsComponent::IsGrounded() const
    {
        return characterController && characterController->IsGrounded();
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void CharacterPhysicsComponent::SetRequestedVelocity(const glm::vec2& velocity)
    {
        if (characterController)
        {
            characterController->SetRequestedVelocity(JPH::Vec3(velocity.x, 0.0f, velocity.y));
        }
    }

    glm::vec3 CharacterPhysicsComponent::GetRequestedVelocity() const
    {
        return characterController ? Vector3From(characterController->GetVelocity()) : glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void CharacterPhysicsComponent::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        SceneComponent::BuildProperties(propertiesInOut);

        REGISTER_PROPERTY(float, radius);
        REGISTER_PROPERTY(float, halfHeight);
    }

    void CharacterPhysicsComponent::PropertyChanged(const Property& property)
    {
        SceneComponent::PropertyChanged(property);

        if (property.GetValuePtr<float>(this) == &radius)
        {
            radius = std::max(radius, 0.0f);
            GenerateController();
            return;
        }

        if (property.GetValuePtr<float>(this) == &halfHeight)
        {
            halfHeight = std::max(halfHeight, 0.0f);
            GenerateController();
            return;
        }
    }

    void CharacterPhysicsComponent::PostConstruct()
    {
        SceneComponent::PostConstruct();

        GenerateController();
    }

    void CharacterPhysicsComponent::OnTransformUpdated()
    {
        SceneComponent::OnTransformUpdated();
        if (characterController)
        {
            // characterController->SetPosition(Vec3From(GetLocalPosition()));
        }
    }

    void CharacterPhysicsComponent::GenerateController()
    {
        if (const World* world = GetWorld())
        {
            characterController = world->GetPhysicsServer()->CreateCharacterController(
                std::max(radius, 0.0f),
                std::max(halfHeight, 0.0f));
            characterController->SetPosition(Vec3From(GetWorldTransform().position));
        }
    }

    void CharacterPhysicsComponent::Tick(float DeltaTime)
    {
        if (characterController)
        {
            const glm::vec3 offset = GetActor()->GetTransform().position - GetWorldTransform().position;
            GetActor()->SetPosition(Vector3From(characterController->GetPosition()) + offset);
        }
    }
} // Vox