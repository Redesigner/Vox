#pragma once

#include <array>

#include <glm/glm.hpp>

#include "voxel/CollisionOctree.h"
#include "voxel/Octree.h"
#include "voxel/Voxel.h"

namespace Vox
{
	class VoxelChunk
	{
	public:
		VoxelChunk();

		void SetVoxel(glm::uvec3 voxelPosition, Voxel voxel);

		void EraseVoxel(glm::uvec3 voxelPosition);

		Voxel GetVoxel(glm::uvec3 voxelPosition) const;

	private:
		std::array<std::array<std::array<Voxel, 32>, 32>, 32> voxels;
		
		Octree::CollisionNode voxelCollisionMask = Octree::CollisionNode(dimensions);

		const unsigned int dimensions = 32;
	};
}