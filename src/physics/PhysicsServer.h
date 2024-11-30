#pragma once

#include <memory>
#include <vector>

#include <Jolt/Jolt.h>

#include <Jolt/Physics/PhysicsSystem.h>

#include "physics/BroadPhaseLayer.h"
#include "physics/ObjectBroadPhaseLayerFilter.h"
#include "physics/ObjectLayerTypes.h"
#include "physics/ObjectPairLayerFilter.h"

namespace JPH
{
	class JobSystem;
}

class PhysicsServer
{
public:
	PhysicsServer();
	~PhysicsServer();

	void Step();

	JPH::BodyID CreateStaticBox(JPH::RVec3 size, JPH::Vec3 position);

	JPH::BodyID CreatePlayerCapsule(float radius, float halfHeight, JPH::Vec3 position);

	JPH::Vec3 GetObjectPosition(const JPH::BodyID& id) const;

private:
	JPH::BodyID CreateStaticShape(JPH::Shape* shape, const JPH::Vec3& position);
	JPH::BodyID CreateDynamicShape(JPH::Shape* shape, const JPH::Vec3& position);

	JPH::PhysicsSystem physicsSystem;

	JPH::uint stepCount = 0;

	std::unique_ptr<JPH::JobSystem> jobSystem;

	std::unique_ptr<JPH::TempAllocatorImpl> tempAllocator;

	std::vector<JPH::BodyID> bodyIds;

	BroadPhaseLayerImplementation broadPhaseLayerImplementation;
	ObjectVsBroadPhaseLayerFilterImplementation	objectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImplementation	objectLayerPairFilter;

	static constexpr float fixedTimeStep = 1.0f / 60.0f;
};