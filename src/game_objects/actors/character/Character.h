#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
#include <memory>

#include "core/datatypes/DelegateHandle.h"
#include "core/datatypes/Ref.h"
#include "../Actor.h"

namespace Vox
{
    class MeshComponent;
}

namespace Vox
{
    class CharacterPhysicsComponent;
}

namespace Vox
{
    class CameraComponent;
	class SpringArm;
    class World;
	struct MeshInstance;

	class Character : public Actor, public Tickable
	{
	public:
		explicit Character(const ObjectInitializer& objectInitializer);
        ~Character() override;

		void Tick(float deltaTime) override;

	    void Play() override;

	private:
        void RotateCamera(int x, int y) const;

	    std::shared_ptr<MeshComponent> mesh;
		std::shared_ptr<CharacterPhysicsComponent> characterController;
	    std::shared_ptr<CameraComponent> cameraComponent;

        DelegateHandle<bool> jumpCallback;
	    DelegateHandle<int, int> mouseLookCallback;

	    IMPLEMENT_OBJECT(Character, Actor)
	};
}
