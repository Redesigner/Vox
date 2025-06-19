#pragma once

#include <map>

// ReSharper disable once CppUnusedIncludeDirective
#include "core/math/Comparators.h"
#include "voxel/Voxel.h"
#include "voxel/VoxelChunk.h"

namespace Vox
{
    class World;

    struct VoxelRaycastResult
    {
        glm::ivec3 voxel;
        glm::ivec3 voxelNormal;
    };

    class VoxelWorld
	{
	public:
        using MapType = std::map<glm::ivec2, VoxelChunk>;
	    explicit VoxelWorld(World* world);
        ~VoxelWorld();

        [[nodiscard]] std::optional<Voxel> GetVoxel(const glm::ivec3& position) const;

        void SetVoxel(const glm::ivec3& position, const Voxel& voxel);

        /**
         * @brief Propagates updates to modified chunks
         */
        void FinalizeUpdate();

        void SaveToFile() const;

        [[nodiscard]] std::optional<VoxelRaycastResult> CastScreenSpaceRay(const glm::ivec2& screenSpace) const;

    private:
        /**
         * @brief Get voxel chunk position and voxel position with that chunk
         * @param position Voxel global position (y is ignored)
         * @return A pair of integer vec2's
         */
        [[nodiscard]] static std::pair<glm::ivec2, glm::ivec2> GetChunkCoords(const glm::ivec3& position);

        [[nodiscard]] std::string WriteString() const;

        MapType voxelChunks;

        World* world;

        std::vector<MapType::iterator> modifiedChunks;
	};
}
