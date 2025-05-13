#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
#include <memory>

#include "core/datatypes/Ref.h"
#include "core/objects/actor/Actor.h"

namespace Vox
{
    class CameraComponent;
	class CharacterController;
	struct MeshInstance;
	class SpringArm;

	class Character : public Actor
	{
	public:
		Character();
        ~Character() override;

		void Update();

	private:
        void RotateCamera(int x, int y) const;

		Ref<CharacterController> characterController;
	    std::shared_ptr<CameraComponent> cameraComponent;

        std::function<void(bool)> jumpCallback;
	    std::function<void(int x, int y)> mouseLookCallback;

	    IMPLEMENT_OBJECT(Character)
	};
}
