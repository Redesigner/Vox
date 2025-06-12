#include "Character.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

#include "core/objects/World.h"
#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"
#include "game_objects/components/CameraComponent.h"
#include "game_objects/components/MeshComponent.h"
#include "game_objects/components/physics/CharacterPhysicsComponent.h"
#include "physics/CharacterController.h"
#include "physics/PhysicsServer.h"
#include "physics/TypeConversions.h"
#include "rendering/Renderer.h"


namespace Vox
{
    Character::Character(const ObjectInitializer& objectInitializer)
	    :Actor(objectInitializer)
	{
        DEFAULT_DISPLAY_NAME();

        characterController = AttachComponent<CharacterPhysicsComponent>(0.5f, 1.0f);
        characterController->SetName("Controller");

	    mesh = AttachComponent<MeshComponent>("witch");
	    mesh->SetPosition({0.0f, -1.5f, 0.0f});
        mesh->SetName("Player Mesh");
        cameraComponent = AttachComponent<CameraComponent>();

        if (!objectInitializer.world)
        {
            return;
        }

		jumpCallback = ServiceLocator::GetInputService()->RegisterKeyboardCallback(SDL_SCANCODE_SPACE, [this](bool pressed) {
			if (pressed && characterController->IsGrounded())
			{
				characterController->AddImpulse(glm::vec3(0.0f, 6.0f, 0.0f));
			}
			});

	    mouseLookCallback = ServiceLocator::GetInputService()->RegisterMouseMotionCallback([this](int xMotion, int yMotion)
        {
	        RotateCamera(xMotion, yMotion);
        });
	}

    Character::~Character() // NOLINT(*-use-equals-default)
    {
        if (InputService* inputService = ServiceLocator::GetInputService())
        {
	        inputService->UnregisterKeyboardCallback(SDL_SCANCODE_SPACE, jumpCallback);
	        inputService->UnregisterMouseMotionCallback(mouseLookCallback);
        }
    }

    void Character::Tick(float deltaTime)
	{
		const glm::vec2 inputVector = ServiceLocator::GetInputService()->GetInputAxisNormalized(
		    Vox::KeyboardInputAxis2D(SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D)
		    );
        const float yaw = cameraComponent->GetWorldTransform().rotation.y;
		const float cos = std::cosf(-yaw);
        const float sin = std::sinf(-yaw);
        const glm::vec2 requestedVelocity = {
            (cos * inputVector.x - sin * inputVector.y) * -4.0f,
            (sin * inputVector.x + cos * inputVector.y) * -4.0f
        };
        characterController->SetRequestedVelocity(requestedVelocity);
        if (requestedVelocity.x != 0.0f && requestedVelocity.y != 0.0f)
        {
            const float characterDirection = glm::atan2(-requestedVelocity.y, requestedVelocity.x);
            mesh->SetRotation({0.0f, characterDirection + glm::pi<float>() / 2.0f, 0.0f});
        }

	    Actor::Tick(deltaTime);
	}

    void Character::Play()
    {
        cameraComponent->Activate();
    }

    void Character::RotateCamera(const int x, const int y) const
    {
	    glm::vec3 rotation = cameraComponent->GetLocalTransform().rotation;
	    rotation.x += static_cast<float>(y) / 500.0f;
	    rotation.x = std::clamp(rotation.x, -1.5f, 1.5f);
	    rotation.y += static_cast<float>(x) / 500.0f;
	    cameraComponent->SetRotation(rotation);
    }
}
