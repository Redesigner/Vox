#pragma once

#include <memory>
#include <vector>

#include <Jolt/Jolt.h>

#include <Jolt/Physics/PhysicsSystem.h>

#include "physics/BroadPhaseLayer.h"
#include "physics/CharacterController.h"
#include "physics/ContactListener.h"
#include "physics/ObjectBroadPhaseLayerFilter.h"
#include "physics/ObjectLayerTypes.h"
#include "physics/ObjectPairLayerFilter.h"

namespace JPH
{
	class JobSystem;
}
namespace Vox
{
	class DebugRenderer;

	class PhysicsServer
	{
	public:
		PhysicsServer();
		~PhysicsServer();

		using CharacterControllerId = unsigned int;

		void Step();

		JPH::BodyID CreateStaticBox(JPH::RVec3 size, JPH::Vec3 position);

		JPH::BodyID CreatePlayerCapsule(float radius, float halfHeight, JPH::Vec3 position);

		CharacterControllerId CreateCharacterController(float radius, float halfHeight);

		JPH::Vec3 GetCharacterControllerVelocity(CharacterControllerId id) const;
		JPH::Vec3 GetCharacterControllerPosition(CharacterControllerId id) const;

		JPH::Vec3 GetObjectPosition(const JPH::BodyID& id) const;

		void RenderDebugShapes();

		Vox::DebugRenderer* GetDebugRenderer() const;

		// Should this be publicly exposed?
		JPH::PhysicsSystem* GetPhysicsSystem();

		// Move these into private methods after restructuring
		JPH::TempAllocator* GetAllocator() const;

	private:
		JPH::BodyID CreateStaticShape(JPH::Shape* shape, const JPH::Vec3& position);
		JPH::BodyID CreateDynamicShape(JPH::Shape* shape, const JPH::Vec3& position);

		JPH::PhysicsSystem physicsSystem;

		JPH::uint stepCount = 0;

		std::unique_ptr<JPH::JobSystem> jobSystem;

		std::unique_ptr<JPH::TempAllocatorImpl> tempAllocator;

		std::unique_ptr<Vox::DebugRenderer> debugRenderer;

		std::vector<JPH::BodyID> bodyIds;

		std::vector<CharacterController> characterControllers;

		BroadPhaseLayerImplementation broadPhaseLayerImplementation;
		ObjectVsBroadPhaseLayerFilterImplementation	objectVsBroadPhaseLayerFilter;
		ObjectLayerPairFilterImplementation	objectLayerPairFilter;

		ContactListener contactListener;

		static constexpr float fixedTimeStep = 1.0f / 60.0f;
	};
}