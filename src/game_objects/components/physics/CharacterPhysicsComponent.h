//
// Created by steph on 6/10/2025.
//

#pragma once

#include "game_objects/components/SceneComponent.h"
#include "physics/CharacterController.h"

namespace Vox
{

    class CharacterPhysicsComponent : public SceneComponent
    {
    public:
        CharacterPhysicsComponent(const ObjectInitializer& objectInitializer);

    private:
        IMPLEMENT_OBJECT(CharacterPhysicsComponent, SceneComponent);

        void OnTransformUpdated() override;

        std::shared_ptr<CharacterController> characterController;
    };

} // Vox
