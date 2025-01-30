#pragma once

#include <memory>
#include <vector>

#include <Jolt/Jolt.h>

#include <Jolt/Physics/PhysicsSystem.h>

#include "core/datatypes/DynamicObjectContainer.h"
#include "core/datatypes/DynamicRef.h"
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

		using CharacterControllerId = unsigned int;
		using SpringArmId = unsigned int;

		void Step();

		JPH::BodyID CreateStaticBox(JPH::RVec3 size, JPH::Vec3 position);

		JPH::BodyID CreatePlayerCapsule(float radius, float halfHeight, JPH::Vec3 position);

		JPH::BodyID CreateCompoundShape(JPH::StaticCompoundShapeSettings* settings);

		DynamicRef<VoxelBody> CreateVoxelBody();

		bool RayCast(JPH::Vec3 origin, JPH::Vec3 direction, RayCastResultNormal& resultOut);


		// Character Controller functions
		// @TODO: move these over to a custom reference class
		CharacterControllerId CreateCharacterController(float radius, float halfHeight);

		JPH::Vec3 GetCharacterControllerVelocity(CharacterControllerId id) const;

		JPH::Vec3 GetCharacterControllerPosition(CharacterControllerId id) const;

		JPH::Quat GetCharacterControllerRotation(CharacterControllerId id) const;

		void SetCharacterControllerYaw(CharacterControllerId id, float yaw);


		SpringArmId CreateSpringArm(CharacterControllerId id);

		SpringArmId CreateSpringArm(JPH::BodyID bodyId);

		void SetSpringArmEulerRotation(SpringArmId id, JPH::Vec3 rotation);

		JPH::Vec3 GetSpringArmEulerRotation(SpringArmId id) const;

		JPH::Vec3 GetSpringArmResult(SpringArmId id) const;

		JPH::Vec3 GetSpringArmOrigin(SpringArmId id) const;


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

		const SpringArm* GetSpringArm(SpringArmId id) const;
		SpringArm* GetSpringArm(SpringArmId id);


		JPH::PhysicsSystem physicsSystem;

		JPH::uint stepCount = 0;

		std::unique_ptr<JPH::JobSystem> jobSystem;

		std::unique_ptr<JPH::TempAllocatorImpl> tempAllocator;

		std::shared_ptr<Vox::DebugRenderer> debugRenderer;

		std::vector<JPH::BodyID> bodyIds;

		std::vector<CharacterController> characterControllers;

		std::vector<SpringArm> springArms;

		DynamicObjectContainer<VoxelBody> voxelBodies;

		BroadPhaseLayerImplementation broadPhaseLayerImplementation;
		ObjectVsBroadPhaseLayerFilterImplementation	objectVsBroadPhaseLayerFilter;
		ObjectLayerPairFilterImplementation	objectLayerPairFilter;

		ContactListener contactListener;

		static constexpr float fixedTimeStep = 1.0f / 60.0f;
	};
}