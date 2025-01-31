#pragma once

#include <memory>
#include <vector>

#include <Jolt/Jolt.h>

#include <Jolt/Physics/PhysicsSystem.h>

#include "core/datatypes/DynamicObjectContainer.h"
#include "core/datatypes/DynamicRef.h"
#include "core/datatypes/Ref.h"
#include "physics/BroadPhaseLayer.h"
#include "physics/CharacterController.h"
#include "physics/ContactListener.h"
#include "physics/ObjectBroadPhaseLayerFilter.h"
#include "physics/ObjectLayerTypes.h"
#include "physics/ObjectPairLayerFilter.h"
#include "physics/RaycastResultNormal.h"
#include "physics/SpringArm.h"
#include "physics/VoxelBody.h"

namespace JPH
{
	class JobSystem;
	class StaticCompoundShapeSettings;
}
namespace Vox
{
	class DebugRenderer;

	class PhysicsServer
	{
	public:
		PhysicsServer();
		~PhysicsServer();

		void Step();

		JPH::BodyID CreateStaticBox(JPH::RVec3 size, JPH::Vec3 position);

		JPH::BodyID CreatePlayerCapsule(float radius, float halfHeight, JPH::Vec3 position);

		JPH::BodyID CreateCompoundShape(JPH::StaticCompoundShapeSettings* settings);

		DynamicRef<VoxelBody> CreateVoxelBody();

		bool RayCast(JPH::Vec3 origin, JPH::Vec3 direction, RayCastResultNormal& resultOut);


		// Character Controller functions
		Ref<CharacterController> CreateCharacterController(float radius, float halfHeight);

		Ref<SpringArm> CreateSpringArm(Ref<CharacterController>& id);

		Ref<SpringArm> CreateSpringArm(JPH::BodyID bodyId);

		JPH::Vec3 GetObjectPosition(const JPH::BodyID& id) const;

		void RenderDebugShapes();

		Vox::DebugRenderer* GetDebugRenderer() const;

		// Should this be publicly exposed?
		JPH::PhysicsSystem* GetPhysicsSystem();

		// Move these into private methods after restructuring
		JPH::TempAllocator* GetAllocator() const;

		void SetDebugRenderer(std::shared_ptr<DebugRenderer> debugRenderer);

	private:
		void StepCharacterControllers();

		void UpdateSpringArms();

		void UpdateVoxelBodies();

		JPH::BodyID CreateStaticShape(JPH::Shape* shape, const JPH::Vec3& position);

		JPH::BodyID CreateDynamicShape(JPH::Shape* shape, const JPH::Vec3& position);

		JPH::PhysicsSystem physicsSystem;

		JPH::uint stepCount = 0;

		std::unique_ptr<JPH::JobSystem> jobSystem;

		std::unique_ptr<JPH::TempAllocatorImpl> tempAllocator;

		std::shared_ptr<Vox::DebugRenderer> debugRenderer;

		std::vector<JPH::BodyID> bodyIds;

		ObjectContainer<CharacterController> characterControllers;

		ObjectContainer<SpringArm> springArms;

		DynamicObjectContainer<VoxelBody> voxelBodies;

		BroadPhaseLayerImplementation broadPhaseLayerImplementation;
		ObjectVsBroadPhaseLayerFilterImplementation	objectVsBroadPhaseLayerFilter;
		ObjectLayerPairFilterImplementation	objectLayerPairFilter;

		ContactListener contactListener;

		static constexpr float fixedTimeStep = 1.0f / 60.0f;
	};
}