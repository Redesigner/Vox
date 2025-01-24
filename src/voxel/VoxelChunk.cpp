#include "VoxelChunk.h"

#include "rendering/Renderer.h"

namespace Vox
{
	VoxelChunk::VoxelChunk(glm::ivec2 chunkLocation, Renderer* renderer)
		:chunkLocation(chunkLocation)
	{
		mesh = renderer->CreateVoxelMesh(chunkLocation);
	}

	void VoxelChunk::SetVoxel(glm::uvec3 voxelPosition, Voxel voxel)
	{
		// Do not use 0 -- it should be reserved to avoid allocating an extra byte in our voxels
		assert((void("Cannot set a voxel materialId to '0'! Use EraseVoxel instead"), voxel.materialId != 0));
		assert(voxelPosition.x < dimensions && voxelPosition.y < dimensions && voxelPosition.z < dimensions);

		if (voxel == voxels[voxelPosition.x][voxelPosition.y][voxelPosition.z])
		{
			return;
		}

		Octree::PhysicsVoxel solidVoxel = Octree::PhysicsVoxel(true);
		voxels[voxelPosition.x][voxelPosition.y][voxelPosition.z] = voxel;
		voxelCollisionMask.SetVoxel(voxelPosition.x - 16, voxelPosition.y - 16, voxelPosition.z - 16, &solidVoxel);
	}

	void VoxelChunk::EraseVoxel(glm::uvec3 voxelPosition)
	{
		assert(voxelPosition.x < dimensions && voxelPosition.y < dimensions && voxelPosition.z < dimensions);
		voxels[voxelPosition.x][voxelPosition.y][voxelPosition.z].materialId = 0;
		voxelCollisionMask.SetVoxel(voxelPosition.x - 16, voxelPosition.y - 16, voxelPosition.z - 16, nullptr);
	}

	Voxel VoxelChunk::GetVoxel(glm::uvec3 voxelPosition) const
	{
		assert(voxelPosition.x < dimensions && voxelPosition.y < dimensions && voxelPosition.z < dimensions);
		return voxels[voxelPosition.x][voxelPosition.y][voxelPosition.z];
	}

	void VoxelChunk::FinalizeUpdate()
	{
		mesh->UpdateData(&voxels);
	}
}