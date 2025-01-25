#pragma once

#include <vector>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

namespace Vox
{
	class VoxelBody
	{
	public:
		VoxelBody();

	private:
		JPH::BodyID bodyId;
	};
}