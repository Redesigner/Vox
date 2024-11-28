#include "PhysicsServer.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

#include "physics/BroadPhaseLayer.h"
#include "physics/ObjectBroadPhaseLayerFilter.h"
#include "physics/ObjectLayerTypes.h"
#include "physics/ObjectPairLayerFilter.h"

#include <thread>

PhysicsServer::PhysicsServer()
{
	JPH::RegisterDefaultAllocator();

	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();

	const JPH::uint cMaxPhysicsJobs = 8;
	const JPH::uint cMaxPhysicsBarriers = 8;

	jobSystem = std::make_unique<JPH::JobSystemThreadPool>(cMaxPhysicsJobs, cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
	tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

	const JPH::uint cMaxBodies = 1024;
	const JPH::uint cNumBodyMutexes = 0;
	const JPH::uint cMaxBodyPairs = 1024;
	const JPH::uint cMaxContactConstraints = 1024;

	BroadPhaseLayerImplementation broadPhaseLayerInterface;
	ObjectVsBroadPhaseLayerFilterImplementation	objectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImplementation	objectLayerPairFilter;

	physicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
		broadPhaseLayerInterface, objectVsBroadPhaseLayerFilter, objectLayerPairFilter);
}

PhysicsServer::~PhysicsServer()
{
	JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
	for (const JPH::BodyID bodyId : bodyIds)
	{
		bodyInterface.RemoveBody(bodyId);
		bodyInterface.DestroyBody(bodyId);
	}

	JPH::UnregisterTypes();

	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;
}

void PhysicsServer::Step()
{
	++stepCount;
	physicsSystem.Update(fixedTimeStep, 1, tempAllocator.get(), jobSystem.get());
}

JPH::BodyID PhysicsServer::CreateStaticBox(JPH::RVec3 size, JPH::Vec3 position)
{
	JPH::BoxShape boxShape = JPH::BoxShape(size / 2.0f);
	return CreateStaticShape(&boxShape, position);
}

JPH::BodyID PhysicsServer::CreatePlayerCapsule(float radius, float halfHeight, JPH::Vec3 position)
{
	JPH::CapsuleShape capsuleShape = JPH::CapsuleShape(radius, halfHeight);
	return CreateDynamicShape(&capsuleShape, position);
}

JPH::BodyID PhysicsServer::CreateStaticShape(JPH::Shape* shape, const JPH::Vec3& position)
{
	JPH::BodyCreationSettings bodyCreationSettings(shape, position, JPH::Quat::sIdentity(), JPH::EMotionType::Static, Physics::CollisionLayer::Static);
	JPH::BodyID bodyId = physicsSystem.GetBodyInterface().CreateAndAddBody(bodyCreationSettings, JPH::EActivation::Activate);
	bodyIds.push_back(bodyId);
	return bodyId;
}

JPH::BodyID PhysicsServer::CreateDynamicShape(JPH::Shape* shape, const JPH::Vec3& position)
{
	JPH::BodyCreationSettings bodyCreationSettings(shape, position, JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Physics::CollisionLayer::Dynamic);
	JPH::BodyID bodyId = physicsSystem.GetBodyInterface().CreateAndAddBody(bodyCreationSettings, JPH::EActivation::Activate);
	bodyIds.push_back(bodyId);
	return bodyId;
}
