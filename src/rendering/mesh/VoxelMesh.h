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
		explicit VoxelMesh(glm::ivec2 position);
		~VoxelMesh();

	    VoxelMesh(const VoxelMesh&) = delete;
	    VoxelMesh(VoxelMesh&& other) noexcept;
	    VoxelMesh& operator =(const VoxelMesh&) = delete;
	    VoxelMesh& operator =(VoxelMesh&& other) noexcept;

		[[nodiscard]] bool NeedsRegeneration() const;

		void Regenerate();

		void UpdateData(std::array<std::array<std::array<Voxel, 32>, 32>, 32>* data);

		glm::mat4x4 GetTransform();

		[[nodiscard]] unsigned int GetMeshId() const;

	private:
		bool dirty = false;

		glm::mat4x4 transform;

		unsigned int voxelDataSsbo, voxelMeshSsbo, meshCounter;
	};
}