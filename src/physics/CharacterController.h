#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

namespace JPH
{
	class PhysicsSystem;
}

namespace Vox
{
	class PhysicsServer;

	class CharacterController
	{
	public:
		CharacterController(JPH::PhysicsSystem* physicsSystem);

		JPH::Vec3 GetPosition() const;

		void Update(float deltaTime, PhysicsServer* physicsServer);

	private:
		JPH::Ref<JPH::CharacterVirtual> character;
		JPH::Ref<JPH::Shape> capsuleShape;
	};
}