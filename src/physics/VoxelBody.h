#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Body/BodyID.h>

#include "Voxel/CollisionOctree.h"

namespace JPH
{
	class StaticCompoundShapeSettings;
}

namespace Vox
{
	class VoxelBody
	{
		friend class PhysicsServer;

	public:
		explicit VoxelBody();
	    ~VoxelBody();

	    VoxelBody(VoxelBody&& other) noexcept;

		void CreateVoxel(glm::uvec3 position);
		void EraseVoxel(glm::uvec3 position);

		JPH::BodyID GetBodyId() const;
		void SetBodyId(JPH::BodyID bodyIdIn);

		JPH::Ref<JPH::StaticCompoundShapeSettings> GetShapeSettings() const;

	    const glm::ivec2& GetChunkPosition() const;

	    glm::ivec2 chunkPosition;

	private:
		JPH::BodyID bodyId;
		std::unique_ptr<Octree::CollisionNode> voxelCollisionMask;
	};
}