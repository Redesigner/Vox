#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

namespace JPH
{
	class PhysicsSystem;
}

namespace Vox
{
	class CharacterController
	{
		friend class PhysicsServer;

	public:
		CharacterController(float inRadius, float inHalfHeight, JPH::PhysicsSystem* physicsSystem);

		JPH::Vec3 GetPosition() const;

		JPH::Vec3 GetVelocity() const;

		void Update(float deltaTime, PhysicsServer* physicsServer);

		float GetRadius() const;
		float GetHalfHeight() const;

	private:
		float radius, halfHeight;

		JPH::Ref<JPH::CharacterVirtual> character;
		JPH::Ref<JPH::Shape> capsuleShape;
	};
}