#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>

#include "core/datatypes/Ref.h"

namespace Vox
{
	class CharacterController;
	struct MeshInstance;

	class Character
	{
	public:
		Character(Ref<CharacterController> characterController, Ref<MeshInstance> meshInstance);

		void Update();

	private:
		Ref<CharacterController> characterController;
		Ref<MeshInstance> meshInstance;

		glm::quat rotation;
		glm::vec3 position;
	};
}