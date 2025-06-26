#include "VoxelChunk.h"

#include <fmt/format.h>

#include "TypedOctree.h"
#include "core/math/Formatting.h"
#include "core/objects/world/World.h"
#include "voxel/Octree.h"
#include "rendering/Renderer.h"
#include "physics/PhysicsServer.h"
#include "rendering/SceneRenderer.h"

namespace Vox
{
	VoxelChunk::VoxelChunk(const glm::ivec2 chunkLocation, const World* world)
		:chunkLocation(chunkLocation)
	{
		mesh = world->GetRenderer()->CreateVoxelMesh(chunkLocation);
		body = world->GetPhysicsServer()->CreateVoxelBody();
	    body->chunkPosition = chunkLocation;
		voxels = std::make_unique<std::array<std::array<std::array<Voxel, chunkSize>, chunkSize>, chunkSize>>();
	}

    VoxelChunk::VoxelChunk(const std::string_view& chunkData, const World* world)
        :chunkLocation()
    {
	    voxels = std::make_unique<std::array<std::array<std::array<Voxel, chunkSize>, chunkSize>, chunkSize>>();
	    size_t cursorPositionR = chunkData.find(',');
	    size_t cursorPositionL = 1;
	    const int chunkX = std::stoi(std::string(chunkData.substr(cursorPositionL, cursorPositionR - cursorPositionL)));

	    cursorPositionL = cursorPositionR + 1;
	    cursorPositionR = chunkData.find(')', cursorPositionL);
	    std::string tempString(chunkData.substr(cursorPositionL, cursorPositionR - cursorPositionL));
	    const int chunkY = std::stoi(tempString);

	    chunkLocation = {chunkX, chunkY};

	    mesh = world->GetRenderer()->CreateVoxelMesh(chunkLocation);
	    body = world->GetPhysicsServer()->CreateVoxelBody();
	    body->chunkPosition = chunkLocation;
	    voxels = std::make_unique<std::array<std::array<std::array<Voxel, chunkSize>, chunkSize>, chunkSize>>();

	    cursorPositionL = cursorPositionR + 1;
	    cursorPositionR = chunkData.find(':', cursorPositionL);

	    const int chunkDataSize = std::stoi(std::string(chunkData.substr(cursorPositionL, cursorPositionR - cursorPositionL)));
	    std::string_view chunkString = chunkData.substr(cursorPositionR + 1, chunkDataSize);
        std::shared_ptr<TypedNode<Voxel>> voxelOctree = TypedNode<Voxel>::FromPacked(chunkString, [](const char c)
        {
            Voxel result;
            result.materialId = c - 48;
            return result;
        });

	    for (int x = 0; x < chunkSize; ++x)
	    {
	        for (int y = 0; y < chunkSize; ++y)
	        {
	            for (int z = 0; z < chunkSize; ++z)
	            {
	                if (Voxel* voxel = voxelOctree->GetData(x - chunkHalfSize, y - chunkHalfSize, z - chunkHalfSize))
	                {
	                    SetVoxel({x, y, z}, *voxel);
	                }
	            }
	        }
	    }
	    FinalizeUpdate();
    }

    void VoxelChunk::SetVoxel(const glm::uvec3 voxelPosition, const Voxel voxel)
	{
		assert(voxelPosition.x < chunkSize && voxelPosition.y < chunkSize && voxelPosition.z < chunkSize);

		if (voxel == voxels->at(voxelPosition.x)[voxelPosition.y][voxelPosition.z])
		{
			return;
		}

		if (voxel.materialId == 0 && voxels->at(voxelPosition.x)[voxelPosition.y][voxelPosition.z].materialId != 0)
		{
			body->EraseVoxel(voxelPosition);
		}
		else if (voxel.materialId != 0 && voxels->at(voxelPosition.x)[voxelPosition.y][voxelPosition.z].materialId == 0)
		{
			body->CreateVoxel(voxelPosition);
		}

	    // Emplace unique
	    if (std::ranges::find(modifiedMaterialIds, voxel.materialId) == modifiedMaterialIds.end())
	    {
	        modifiedMaterialIds.emplace_back(voxel.materialId);
	    }

		voxels->at(voxelPosition.x)[voxelPosition.y][voxelPosition.z] = voxel;
	}

	Voxel VoxelChunk::GetVoxel(const glm::uvec3 voxelPosition) const
	{
		assert(voxelPosition.x < chunkSize && voxelPosition.y < chunkSize && voxelPosition.z < chunkSize);
		return voxels->at(voxelPosition.x)[voxelPosition.y][voxelPosition.z];
	}

	void VoxelChunk::FinalizeUpdate()
	{
		mesh->UpdateData(voxels.get(), modifiedMaterialIds);
	    modifiedMaterialIds.clear();
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

    glm::ivec2 VoxelChunk::GetChunkLocation() const
    {
        return chunkLocation;
    }

    std::string VoxelChunk::WriteString() const
    {
        TypedNode<Voxel> octree(chunkSize);
	    for (int x = 0 ; x < chunkSize; ++x)
	    {
	        for (int y = 0; y < chunkSize; ++y)
	        {
	            for (int z = 0; z < chunkSize; ++z)
	            {
	                const Voxel& voxel = voxels->at(x).at(y).at(z);
	                if (voxel.materialId == 0)
	                {
	                    continue;
	                }
	                octree.SetData(x - chunkHalfSize, y - chunkHalfSize, z - chunkHalfSize, voxel);
	            }
	        }
	    }
	    const std::string data = octree.GetPacked([](const Voxel& voxel){ return voxel.materialId + 48; });
	    const std::string chunk = {data.begin(), data.end()};
	    return fmt::format("({},{}){}:{}", chunkLocation.x, chunkLocation.y, chunk.size(), chunk);
    }
}
