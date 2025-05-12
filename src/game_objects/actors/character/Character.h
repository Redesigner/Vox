#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
#include <memory>

#include "core/datatypes/Ref.h"
#include "core/objects/actor/Actor.h"

namespace Vox
{
	class Camera;
    class CameraComponent;
	class CharacterController;
	struct MeshInstance;
	class SpringArm;

	class Character : public Actor
	{
	public:
		Character();

		void Update();

	private:
		Ref<CharacterController> characterController;
		Ref<MeshInstance> meshInstance;
		Ref<Camera> camera;
		Ref<SpringArm> springArm;
	    std::weak_ptr<CameraComponent> cameraComponent;

		glm::quat rotation;
		glm::vec3 position;
	};
}
