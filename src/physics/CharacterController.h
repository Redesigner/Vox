#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <mutex>

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

		[[nodiscard]] JPH::Vec3 GetPosition() const;

		[[nodiscard]] JPH::Vec3 GetVelocity() const;

		[[nodiscard]] JPH::Quat GetRotation() const;

	    [[nodiscard]] const JPH::Vec3& GetRequestedVelocity() const;

		[[nodiscard]] bool IsGrounded() const;

		[[nodiscard]] float GetRadius() const;

		[[nodiscard]] float GetHalfHeight() const;

		void AddImpulse(JPH::Vec3 impulse);

		void Update(float deltaTime, PhysicsServer* physicsServer);

	    void SetPosition(JPH::Vec3 position);

	    void SetRequestedVelocity(const JPH::Vec3& velocity);

	private:
		float radius, halfHeight;

	    std::mutex characterMutex;

		bool grounded = true;

		JPH::Vec3 pendingImpulses;

		JPH::Vec3 requestedVelocity;

		JPH::Ref<JPH::CharacterVirtual> character;
		JPH::Ref<JPH::Shape> capsuleShape;
	};
}