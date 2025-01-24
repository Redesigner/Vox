#pragma once

#include <array>

#include <glm/glm.hpp>

#include "core/datatypes/Ref.h"
#include "voxel/CollisionOctree.h"
#include "voxel/Octree.h"
#include "voxel/Voxel.h"

namespace Vox
{
	class Renderer;
	class VoxelMesh;

	class VoxelChunk
	{
	public:
		// @TODO: Change how our dependency is injected here? We need this to create our mesh, not for any other reason
		VoxelChunk(glm::ivec2 chunkLocation, Renderer* renderer);

		void SetVoxel(glm::uvec3 voxelPosition, Voxel voxel);

		void EraseVoxel(glm::uvec3 voxelPosition);

		Voxel GetVoxel(glm::uvec3 voxelPosition) const;

		void FinalizeUpdate();

	private:
		glm::ivec2 chunkLocation;

		Ref<VoxelMesh> mesh;

		std::array<std::array<std::array<Voxel, 32>, 32>, 32> voxels;

		const unsigned int dimensions = 32;
		
		Octree::CollisionNode voxelCollisionMask = Octree::CollisionNode(dimensions);
	};
}