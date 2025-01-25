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
		VoxelBody(unsigned int dimensions);

		void SetVoxel(glm::uvec3 position);
		void EraseVoxel(glm::uvec3 position);

		JPH::Ref<JPH::StaticCompoundShapeSettings> GetShapeSettings();

	private:
		JPH::BodyID bodyId;
		Octree::CollisionNode voxelCollisionMask;
	};
}