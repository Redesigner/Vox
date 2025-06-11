//
// Created by steph on 6/10/2025.
//

#include "CharacterPhysicsComponent.h"

#include "physics/TypeConversions.h"

namespace Vox
{
    CharacterPhysicsComponent::CharacterPhysicsComponent(const ObjectInitializer& objectInitializer)
        :SceneComponent(objectInitializer)
    {
        if (objectInitializer.world)
        {

        }
    }

    void CharacterPhysicsComponent::OnTransformUpdated()
    {
        SceneComponent::OnTransformUpdated();
        characterController->SetPosition(Vec3From(GetLocalPosition()));
    }
} // Vox