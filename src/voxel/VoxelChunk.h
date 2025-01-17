#pragma once

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

		void SetVoxel(glm::ivec3 voxelPosition, Voxel voxel);

		void EraseVoxel(glm::ivec3 voxelPosition);

		Voxel GetVoxel(glm::ivec3 voxelPosition) const;

	private:

		Octree::Node voxels = Octree::Node(dimensions);
		Octree::CollisionNode voxelCollisionMask = Octree::CollisionNode(dimensions);

		const unsigned int dimensions = 32;
	};
}