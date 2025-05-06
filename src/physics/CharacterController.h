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
		CharacterController(float inRadius, float inHalfHeight, JPH::PhysicsSystem* physicsSystem);

		JPH::Vec3 GetPosition() const;

		JPH::Vec3 GetVelocity() const;

		JPH::Quat GetRotation() const;

		void AddImpulse(JPH::Vec3 impulse);

		bool IsGrounded() const;

		void Update(float deltaTime, PhysicsServer* physicsServer);

		float GetRadius() const;

		float GetHalfHeight() const;

		JPH::Vec3 requestedVelocity;

	private:
		float radius, halfHeight;

		bool grounded = true;

		JPH::Vec3 pendingImpulses;

		JPH::Ref<JPH::CharacterVirtual> character;
		JPH::Ref<JPH::Shape> capsuleShape;
	};
}