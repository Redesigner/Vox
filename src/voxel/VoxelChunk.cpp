#include "VoxelChunk.h"

namespace Vox
{
	VoxelChunk::VoxelChunk()
	{
	}

	void VoxelChunk::SetVoxel(glm::uvec3 voxelPosition, Voxel voxel)
	{
		// Do not use 0 -- it should be reserved to avoid allocating an extra byte in our voxels
		assert(voxel.materialId != 0, "Cannot set a voxel materialId to '0'! Use EraseVoxel instead");
		assert(voxelPosition.x < dimensions && voxelPosition.y < dimensions && voxelPosition.z < dimensions);
		Octree::PhysicsVoxel solidVoxel = Octree::PhysicsVoxel(true);
		voxels[voxelPosition.x][voxelPosition.y][voxelPosition.z] = voxel;
		voxelCollisionMask.SetVoxel(voxelPosition.x, voxelPosition.y, voxelPosition.z, &solidVoxel);
	}

	void VoxelChunk::EraseVoxel(glm::uvec3 voxelPosition)
	{
		assert(voxelPosition.x < dimensions && voxelPosition.y < dimensions && voxelPosition.z < dimensions);
		voxels[voxelPosition.x][voxelPosition.y][voxelPosition.z].materialId = 0;
		voxelCollisionMask.SetVoxel(voxelPosition.x, voxelPosition.y, voxelPosition.z, nullptr);
	}

	Voxel VoxelChunk::GetVoxel(glm::uvec3 voxelPosition) const
	{
		assert(voxelPosition.x < dimensions && voxelPosition.y < dimensions && voxelPosition.z < dimensions);
		return voxels[voxelPosition.x][voxelPosition.y][voxelPosition.z];
	}
}