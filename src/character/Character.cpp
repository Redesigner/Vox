#include "Character.h"

#include <glm/ext/matrix_transform.hpp>

#include "physics/CharacterController.h"
#include "physics/TypeConversions.h"
#include "rendering/mesh/MeshInstance.h"

namespace Vox
{
	Character::Character(Ref<CharacterController> characterController, Ref<MeshInstance> meshInstance)
		:characterController(characterController), meshInstance(meshInstance)
	{
	}

	void Character::Update()
	{
		glm::vec3 controllerPosition = glm::vec3(Vector3From(characterController->GetPosition()));
		controllerPosition.y -= 1.25f;
		meshInstance->SetTransform(glm::translate(glm::identity<glm::mat4x4>(), controllerPosition));
	}
}