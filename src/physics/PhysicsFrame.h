#pragma once

#include <chrono>
#include <unordered_map>

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Body/BodyID.h>

namespace Vox
{
	struct Transform
	{
		JPH::Vec3 position;
		JPH::Quat rotation;
	};

	struct PhysicsFrame
	{
		std::chrono::time_point frameCreationTime;
		std::unordered_map<JPH::BodyID, Transform>
	};
}