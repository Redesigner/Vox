#pragma once

#include <vector>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

namespace Vox
{
	class VoxelChunkBody
	{
	public:


	private:
		JPH::BodyID bodyId;
	};

	struct VoxelChunkRef
	{
	public:
		VoxelChunkRef(std::vector<VoxelChunkBody>* container, size_t index);

	private:

	};
}