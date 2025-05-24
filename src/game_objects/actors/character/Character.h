#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
#include <memory>

#include "core/datatypes/DelegateHandle.h"
#include "core/datatypes/Ref.h"
#include "core/objects/actor/Actor.h"

namespace Vox
{
    class CameraComponent;
	class CharacterController;
	class SpringArm;
    class World;
	struct MeshInstance;

	class Character : public Actor
	{
	public:
		Character(const ObjectInitializer& objectInitializer);
        ~Character() override;

		void Tick(float deltaTime) override;

	private:
        void RotateCamera(int x, int y) const;

		Ref<CharacterController> characterController;
	    std::shared_ptr<CameraComponent> cameraComponent;

        std::function<void(bool)> jumpCallback;
	    DelegateHandle<int, int> mouseLookCallback;

	    IMPLEMENT_OBJECT(Character)
	};
}
