#pragma once
#include <map>

#include "voxel/Voxel.h"
#include "voxel/VoxelChunk.h"

namespace Vox
{
    class World;

    class VoxelWorld
	{
	public:
        using MapType = std::map<std::pair<int, int>, VoxelChunk>;
	    explicit VoxelWorld(World* world);
        ~VoxelWorld();

        [[nodiscard]] std::optional<Voxel> GetVoxel(const glm::ivec3& position) const;

        void SetVoxel(const glm::ivec3& position, const Voxel& voxel);

        /**
         * @brief Propagates updates to modified chunks
         */
        void FinalizeUpdate();

    private:
        /**
         * @brief Get voxel chunk position and voxel position with that chunk
         * @param position Voxel global position (y is ignored)
         * @return A pair of integer vec2's
         */
        [[nodiscard]] static std::pair<glm::ivec2, glm::ivec2> GetChunkCoords(const glm::ivec3& position);

        MapType voxelChunks;

        World* world;

        std::vector<MapType::iterator> modifiedChunks;
	};
}
