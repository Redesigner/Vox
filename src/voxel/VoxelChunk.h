#pragma once

#include <array>
#include <memory>

#include <glm/glm.hpp>

#include "core/datatypes/DynamicRef.h"
#include "physics/VoxelBody.h"
#include "voxel/CollisionOctree.h"
#include "voxel/Voxel.h"

namespace Vox
{
    class World;
}

namespace Vox
{
    class SceneRenderer;
    class Renderer;
	class PhysicsServer;
	class VoxelMesh;

	class VoxelChunk
	{
	public:
		VoxelChunk(glm::ivec2 chunkLocation, const World* world);

	    VoxelChunk(const std::string_view& chunkData, const World* world);

		void SetVoxel(glm::uvec3 voxelPosition, Voxel voxel);

		[[nodiscard]] Voxel GetVoxel(glm::uvec3 voxelPosition) const;

		void FinalizeUpdate();

		static constexpr int chunkSize = 32;
	    static constexpr int chunkHalfSize = chunkSize / 2;

	    static glm::vec3 CalculatePosition(const glm::ivec2& position);

	    [[nodiscard]] glm::ivec2 GetChunkLocation() const;

	    [[nodiscard]] std::string WriteString() const;

	private:
		glm::ivec2 chunkLocation;

		DynamicRef<VoxelMesh> mesh;

		DynamicRef<VoxelBody> body;

		std::unique_ptr<std::array<std::array<std::array<Voxel, chunkSize>, chunkSize>, chunkSize>> voxels;

	    std::vector<int> modifiedMaterialIds;

		Octree::CollisionNode voxelCollisionMask = Octree::CollisionNode(chunkSize);
	};
}
