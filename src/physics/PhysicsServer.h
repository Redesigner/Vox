#pragma once

#include <memory>

#include <Jolt/Jolt.h>

#include <Jolt/Physics/PhysicsSystem.h>

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

private:
	JPH::PhysicsSystem physicsSystem;

	JPH::uint stepCount = 0;

	std::unique_ptr<JPH::JobSystem> jobSystem;

	std::unique_ptr<JPH::TempAllocatorImpl> tempAllocator;

	static constexpr float fixedTimeStep = 1.0f / 60.0f;
};