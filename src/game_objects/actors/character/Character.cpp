#include "Character.h"

#include <glm/ext/matrix_transform.hpp>

#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"
#include "game_objects/components/CameraComponent.h"
#include "game_objects/components/MeshComponent.h"
#include "physics/CharacterController.h"
#include "physics/PhysicsServer.h"
#include "physics/SpringArm.h"
#include "physics/TypeConversions.h"
#include "rendering/Camera.h"
#include "rendering/mesh/MeshInstance.h"
#include "rendering/Renderer.h"


namespace Vox
{
	Character::Character()
	{
		characterController = ServiceLocator::GetPhysicsServer()->CreateCharacterController(0.5f, 1.0f);
		springArm = ServiceLocator::GetPhysicsServer()->CreateSpringArm(characterController);

	    AttachComponent<MeshComponent>("witch");
        cameraComponent = AttachComponent<CameraComponent>();
        cameraComponent.lock()->Activate();

		jumpCallback = ServiceLocator::GetInputService()->RegisterKeyboardCallback(SDL_SCANCODE_SPACE, [this](bool pressed) {
			if (pressed && characterController->IsGrounded())
			{
				characterController->AddImpulse(JPH::Vec3(0.0f, 6.0f, 0.0f));
			}
			});
	}

    Character::~Character()
    {
	    ServiceLocator::GetInputService()->UnregisterKeyboardCallback(SDL_SCANCODE_SPACE, jumpCallback);
    }

    void Character::Update()
	{
		glm::vec2 inputVector = ServiceLocator::GetInputService()->GetInputAxisNormalized(Vox::KeyboardInputAxis2D(SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D));
		float yaw = cameraComponent.lock()->GetLocalTransform().rotation.y;
		float cos = std::cosf(yaw);
		float sin = std::sinf(yaw);
		characterController->requestedVelocity.SetX((cos * inputVector.x - sin * inputVector.y) * -4.0f);
		characterController->requestedVelocity.SetZ((sin * inputVector.x + cos * inputVector.y) * -4.0f);

		glm::vec3 controllerPosition = glm::vec3(Vector3From(characterController->GetPosition()));
		controllerPosition.y -= 1.25f;
		JPH::Quat characterRotation = characterController->GetRotation();
		glm::quat characterQuat = glm::quat(characterRotation.GetX(), characterRotation.GetY(), characterRotation.GetZ(), characterRotation.GetW());
		glm::mat4x4 rotation = glm::toMat4(characterQuat);
		SetPosition(controllerPosition);
	}
}