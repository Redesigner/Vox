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
	    explicit VoxelWorld(World* world);
        ~VoxelWorld();

        [[nodiscard]] std::optional<Voxel> GetVoxel(const glm::ivec3& position) const;

        void SetVoxel(const glm::ivec3& position, const Voxel& voxel);

    private:
        /**
         * @brief Get voxel chunk position and voxel position with that chunk
         * @param position Voxel global position (y is ignored)
         * @return A pair of integer vec2's
         */
        [[nodiscard]] static std::pair<glm::ivec2, glm::ivec2> GetChunkCoords(const glm::ivec3& position);

        std::map<std::pair<int, int>, VoxelChunk> voxelChunks;

        World* world;
	};
}
