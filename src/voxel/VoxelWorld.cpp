#include "VoxelWorld.h"

namespace Vox
{
    VoxelWorld::VoxelWorld(World* world)
        :world(world)
    {
    }

    VoxelWorld::~VoxelWorld()
    = default;

    std::optional<Voxel> VoxelWorld::GetVoxel(const glm::ivec3& position) const
    {
        auto [chunkPosition, voxelPosition] = GetChunkCoords(position);
        const auto chunkIterator = voxelChunks.find({chunkPosition.x, chunkPosition.y});
        if (chunkIterator == voxelChunks.end())
        {
            return std::nullopt;
        }

        return chunkIterator->second.GetVoxel({voxelPosition.x, position.y, voxelPosition.y});
    }

    std::pair<glm::ivec2, glm::ivec2> VoxelWorld::GetChunkCoords(const glm::ivec3& position)
    {
        auto [chunkX, voxelX] = std::div(position.x, VoxelChunk::chunkSize);
        auto [chunkY, voxelY] = std::div(position.z, VoxelChunk::chunkSize);
        if (voxelX < 0)
        {
            voxelX += VoxelChunk::chunkSize;
            --chunkX;
        }

        if (voxelY < 0)
        {
            voxelY += VoxelChunk::chunkSize;
            --chunkY;
        }

        return {{chunkX, chunkY}, {voxelX, voxelY}};
    }
}
