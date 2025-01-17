#include "VoxelChunk.h"

namespace Vox
{
	VoxelChunk::VoxelChunk()
	{
	}

	void VoxelChunk::SetVoxel(glm::ivec3 voxelPosition, Voxel voxel)
	{
		Octree::PhysicsVoxel solidVoxel = Octree::PhysicsVoxel(true);
		voxels.SetVoxel(voxelPosition.x, voxelPosition.y, voxelPosition.z, &voxel);
		voxelCollisionMask.SetVoxel(voxelPosition.x, voxelPosition.y, voxelPosition.z, &solidVoxel);
	}

	void VoxelChunk::EraseVoxel(glm::ivec3 voxelPosition)
	{
		voxels.SetVoxel(voxelPosition.x, voxelPosition.y, voxelPosition.z, nullptr);
		voxelCollisionMask.SetVoxel(voxelPosition.x, voxelPosition.y, voxelPosition.z, nullptr);
	}

	Voxel VoxelChunk::GetVoxel(glm::ivec3 voxelPosition) const
	{
		return *voxels.GetVoxel(voxelPosition.x, voxelPosition.y, voxelPosition.z);
	}
}