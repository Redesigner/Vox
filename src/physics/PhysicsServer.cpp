#include "PhysicsServer.h"

#include <thread>

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include "physics/BroadPhaseLayer.h"
#include "physics/ObjectBroadPhaseLayerFilter.h"
#include "physics/ObjectPairLayerFilter.h"

PhysicsServer::PhysicsServer()
{
	JPH::RegisterDefaultAllocator();

	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();

	const JPH::uint cMaxPhysicsJobs = 8;
	const JPH::uint cMaxPhysicsBarriers = 8;

	jobSystem = std::make_unique<JPH::JobSystem>(cMaxPhysicsJobs, cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
	tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

	const JPH::uint cMaxBodies = 1024;
	const JPH::uint cNumBodyMutexes = 0;
	const JPH::uint cMaxBodyPairs = 1024;
	const JPH::uint cMaxContactConstraints = 1024;

	BroadPhaseLayerImplementation broadPhaseLayerInterface;
	ObjectVsBroadPhaseLayerFilterImplementation objectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImplementation objectLayerPairFilter;

	physicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
		broadPhaseLayerInterface, objectVsBroadPhaseLayerFilter, objectLayerPairFilter);
}

PhysicsServer::~PhysicsServer()
{
	JPH::UnregisterTypes();

	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;
}

void PhysicsServer::Step()
{
	++stepCount;
	physicsSystem.Update(fixedTimeStep, 1, tempAllocator.get(), jobSystem.get());
}
