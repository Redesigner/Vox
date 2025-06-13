#pragma once

#include <array>
#include <memory>

#include <glm/glm.hpp>

#include "core/datatypes/DynamicRef.h"
#include "physics/VoxelBody.h"
#include "voxel/CollisionOctree.h"
#include "voxel/Octree.h"
#include "voxel/Voxel.h"

namespace Vox
{
    class SceneRenderer;
    class Renderer;
	class PhysicsServer;
	class VoxelMesh;

	class VoxelChunk
	{
	public:
		// @TODO: Change how our dependency is injected here? We need this to create our mesh, not for any other reason
		VoxelChunk(glm::ivec2 chunkLocation, PhysicsServer* physicsServer, SceneRenderer* renderer);

		void SetVoxel(glm::uvec3 voxelPosition, Voxel voxel);

		[[nodiscard]] Voxel GetVoxel(glm::uvec3 voxelPosition) const;

		void FinalizeUpdate();

		static constexpr unsigned int chunkSize = 32;

	private:
		glm::ivec2 chunkLocation;

		DynamicRef<VoxelMesh> mesh;

		DynamicRef<VoxelBody> body;

		std::unique_ptr<std::array<std::array<std::array<Voxel, chunkSize>, chunkSize>, chunkSize>> voxels;

		Octree::CollisionNode voxelCollisionMask = Octree::CollisionNode(chunkSize);
	};
}