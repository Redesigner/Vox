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
	    body->chunkPosition = chunkLocation;
		voxels = std::make_unique<std::array<std::array<std::array<Voxel, chunkSize>, chunkSize>, chunkSize>>();
	}

	void VoxelChunk::SetVoxel(glm::uvec3 voxelPosition, Voxel voxel)
	{
		assert(voxelPosition.x < chunkSize && voxelPosition.y < chunkSize && voxelPosition.z < chunkSize);

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
		assert(voxelPosition.x < chunkSize && voxelPosition.y < chunkSize && voxelPosition.z < chunkSize);
		return voxels->at(voxelPosition.x)[voxelPosition.y][voxelPosition.z];
	}

	void VoxelChunk::FinalizeUpdate()
	{
		mesh->UpdateData(voxels.get());
		mesh.MarkDirty();
		body.MarkDirty();
	}

    glm::vec3 VoxelChunk::CalculatePosition(const glm::ivec2& position)
    {
	    return {
	        position.x * chunkSize - chunkHalfSize,
            -chunkHalfSize,
            position.y * chunkSize - chunkHalfSize
	    };
    }
}
