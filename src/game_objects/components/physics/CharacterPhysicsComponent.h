//
// Created by steph on 6/10/2025.
//

#pragma once

#include "core/objects/Tickable.h"
#include "game_objects/components/SceneComponent.h"
#include "physics/CharacterController.h"

namespace Vox
{

    class CharacterPhysicsComponent : public SceneComponent, public Tickable
    {
    public:
        CharacterPhysicsComponent(const ObjectInitializer& objectInitializer);
        CharacterPhysicsComponent(const ObjectInitializer& objectInitializer, float radius, float halfHeight);

        void AddImpulse(glm::vec3 impulse);

        [[nodiscard]] bool IsGrounded() const;

        void SetRequestedVelocity(const glm::vec2& velocity);

        [[nodiscard]] glm::vec3 GetRequestedVelocity() const;

        void SetDirectPosition(const glm::vec3& position);

        void PostConstruct() override;

        TickOrder GetTickOrder() override;

    protected:
        void BuildProperties(std::vector<Property>& propertiesInOut) override;

        void PropertyChanged(const Property& property) override;

    private:
        IMPLEMENT_OBJECT(CharacterPhysicsComponent, SceneComponent);

        void OnTransformUpdated() override;

        void GenerateController();

        void Tick(float DeltaTime) override;

        std::shared_ptr<CharacterController> characterController;

        float radius;

        float halfHeight;
    };

} // Vox
