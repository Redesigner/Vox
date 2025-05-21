#include "VoxelChunk.h"

#include "rendering/Renderer.h"
#include "physics/PhysicsServer.h"
#include "rendering/SceneRenderer.h"

namespace Vox
{
	VoxelChunk::VoxelChunk(glm::ivec2 chunkLocation, PhysicsServer* physicsServer, SceneRenderer* renderer)
		:chunkLocation(chunkLocation)
	{
		mesh = renderer->CreateVoxelMesh(chunkLocation);
		body = physicsServer->CreateVoxelBody();
		voxels = std::make_unique<std::array<std::array<std::array<Voxel, 32>, 32>, 32>>();
	}

	void VoxelChunk::SetVoxel(glm::uvec3 voxelPosition, Voxel voxel)
	{
		assert(voxelPosition.x < dimensions && voxelPosition.y < dimensions && voxelPosition.z < dimensions);

		if (voxel == voxels->at(voxelPosition.x)[voxelPosition.y][voxelPosition.z])
		{
			return;
		}

		if (voxel.materialId == 0 && voxels->at(voxelPosition.x)[voxelPosition.y][voxelPosition.z].materialId != 0)
		{
			body->EraseVoxel(voxelPosition);
			// body.MarkDirty();
		}
		else if (voxel.materialId != 0 && voxels->at(voxelPosition.x)[voxelPosition.y][voxelPosition.z].materialId == 0)
		{
			body->CreateVoxel(voxelPosition);
			// body.MarkDirty();
		}

		voxels->at(voxelPosition.x)[voxelPosition.y][voxelPosition.z] = voxel;
	}

	Voxel VoxelChunk::GetVoxel(glm::uvec3 voxelPosition) const
	{
		assert(voxelPosition.x < dimensions && voxelPosition.y < dimensions && voxelPosition.z < dimensions);
		return voxels->at(voxelPosition.x)[voxelPosition.y][voxelPosition.z];
	}

	void VoxelChunk::FinalizeUpdate()
	{
		mesh->UpdateData(voxels.get());
		mesh.MarkDirty();
		body.MarkDirty();
	}
}
