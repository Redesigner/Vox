#include "VoxelChunk.h"

#include "rendering/Renderer.h"
#include "physics/PhysicsServer.h"

namespace Vox
{
	VoxelChunk::VoxelChunk(glm::ivec2 chunkLocation, PhysicsServer* physicsServer, Renderer* renderer)
		:chunkLocation(chunkLocation)
	{
		mesh = renderer->CreateVoxelMesh(chunkLocation);
		body = physicsServer->CreateVoxelBody();
	}

	void VoxelChunk::SetVoxel(glm::uvec3 voxelPosition, Voxel voxel)
	{
		assert(voxelPosition.x < dimensions && voxelPosition.y < dimensions && voxelPosition.z < dimensions);

		if (voxel == voxels[voxelPosition.x][voxelPosition.y][voxelPosition.z])
		{
			return;
		}

		if (voxel.materialId == 0 && voxels[voxelPosition.x][voxelPosition.y][voxelPosition.z].materialId != 0)
		{
			body->EraseVoxel(voxelPosition);
			// body.MarkDirty();
		}
		else if (voxel.materialId != 0 && voxels[voxelPosition.x][voxelPosition.y][voxelPosition.z].materialId == 0)
		{
			body->CreateVoxel(voxelPosition);
			// body.MarkDirty();
		}

		voxels[voxelPosition.x][voxelPosition.y][voxelPosition.z] = voxel;
	}

	Voxel VoxelChunk::GetVoxel(glm::uvec3 voxelPosition) const
	{
		assert(voxelPosition.x < dimensions && voxelPosition.y < dimensions && voxelPosition.z < dimensions);
		return voxels[voxelPosition.x][voxelPosition.y][voxelPosition.z];
	}

	void VoxelChunk::FinalizeUpdate()
	{
		mesh->UpdateData(&voxels);
		mesh.MarkDirty();
		body.MarkDirty();
	}
}