#include "Character.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"
#include "game_objects/components/CameraComponent.h"
#include "game_objects/components/MeshComponent.h"
#include "physics/CharacterController.h"
#include "physics/PhysicsServer.h"
#include "physics/TypeConversions.h"
#include "rendering/Renderer.h"


namespace Vox
{
	Character::Character(World* world)
	    :Actor(world)
	{
		characterController = ServiceLocator::GetPhysicsServer()->CreateCharacterController(0.5f, 1.0f);

	    auto mesh = AttachComponent<MeshComponent>("witch");
	    mesh->SetPosition({0.0f, -1.5f, 0.0f});
        cameraComponent = AttachComponent<CameraComponent>();
        cameraComponent->Activate();

		jumpCallback = ServiceLocator::GetInputService()->RegisterKeyboardCallback(SDL_SCANCODE_SPACE, [this](bool pressed) {
			if (pressed && characterController->IsGrounded())
			{
				characterController->AddImpulse(JPH::Vec3(0.0f, 6.0f, 0.0f));
			}
			});

	    mouseLookCallback = ServiceLocator::GetInputService()->RegisterMouseMotionCallback([this](int xMotion, int yMotion)
        {
	        RotateCamera(xMotion, yMotion);
        });
	}

    Character::~Character()
    {
	    ServiceLocator::GetInputService()->UnregisterKeyboardCallback(SDL_SCANCODE_SPACE, jumpCallback);
	    ServiceLocator::GetInputService()->UnregisterMouseMotionCallback(mouseLookCallback);
    }

    void Character::Tick(float deltaTime)
	{
		const glm::vec2 inputVector = ServiceLocator::GetInputService()->GetInputAxisNormalized(
		    Vox::KeyboardInputAxis2D(SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D)
		    );
        float pitch, yaw, roll;
	    glm::extractEulerAngleYXZ(cameraComponent->GetWorldTransform().GetMatrix(), yaw, pitch, roll);
		const float cos = std::cosf(-yaw);
        const float sin = std::sinf(-yaw);
		characterController->requestedVelocity.SetX((cos * inputVector.x - sin * inputVector.y) * -4.0f);
		characterController->requestedVelocity.SetZ((sin * inputVector.x + cos * inputVector.y) * -4.0f);

		SetPosition(Vector3From(characterController->GetPosition()));

	    Actor::Tick(deltaTime);
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
