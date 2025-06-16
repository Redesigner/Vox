#include "VoxelWorld.h"

#include "core/logging/Logging.h"
#include "core/objects/World.h"
#include "core/math/Formatting.h"

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

    void VoxelWorld::SetVoxel(const glm::ivec3& position, const Voxel& voxel)
    {
        auto [chunkPosition, voxelPosition] = GetChunkCoords(position);
        const auto chunkIterator = voxelChunks.find({chunkPosition.x, chunkPosition.y});
        if (chunkIterator == voxelChunks.end())
        {
            // If the chunk doesn't exist, allocate a new one
            // Default map can't hash ivec2, use std::pair here instead
            const auto [iterator, success] = voxelChunks.emplace(
                std::pair(chunkPosition.x, chunkPosition.y),
                VoxelChunk(chunkPosition, world->GetPhysicsServer().get(), world->GetRenderer().get())
            );
            VoxLog(Display, Game, "Allocating new voxel at coords '{}'", chunkPosition);
            iterator->second.SetVoxel({voxelPosition.x, position.y, voxelPosition.y}, voxel);

            if (std::ranges::find(modifiedChunks, iterator) == modifiedChunks.end())
            {
                modifiedChunks.push_back(iterator);
            }
            return;
        }

        chunkIterator->second.SetVoxel({voxelPosition.x, position.y, voxelPosition.y}, voxel);

        if (std::ranges::find(modifiedChunks, chunkIterator) == modifiedChunks.end())
        {
            modifiedChunks.push_back(chunkIterator);
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void VoxelWorld::FinalizeUpdate()
    {
        for (const auto& modifiedChunk : modifiedChunks)
        {
            modifiedChunk->second.FinalizeUpdate();
        }
        modifiedChunks.clear();
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
