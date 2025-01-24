#pragma once

#include <array>
#include <vector>

#include <glm/glm.hpp>

#include "core/datatypes/Ref.h"
#include "voxel/Voxel.h"

namespace Vox
{
	class VoxelMesh
	{
	public:
		VoxelMesh(glm::ivec2 position);
		~VoxelMesh();

		bool NeedsRegeneration() const;

		void Regenerate();

		void UpdateData(std::array<std::array<std::array<Voxel, 32>, 32>, 32>* data);

		glm::mat4x4 GetTransform();

		unsigned int GetMeshId() const;

	private:
		bool dirty = false;

		glm::mat4x4 transform;

		unsigned int voxelDataSsbo, voxelMeshSsbo, meshCounter;
	};
}