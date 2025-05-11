#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>

#include "core/datatypes/Ref.h"

namespace Vox
{
	class Camera;
	class CharacterController;
	struct MeshInstance;
	class SpringArm;

	class Character
	{
	public:
		Character();

		void Update();

	private:
		Ref<CharacterController> characterController;
		Ref<MeshInstance> meshInstance;
		Ref<Camera> camera;
		Ref<SpringArm> springArm;

		glm::quat rotation;
		glm::vec3 position;
	};
}