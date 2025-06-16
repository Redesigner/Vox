#pragma once

#include <array>
#include <vector>

#include <glm/glm.hpp>

#include "core/datatypes/Ref.h"
#include "voxel/Voxel.h"

namespace Vox
{
    class ComputeShader;
}

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

		void Regenerate(const ComputeShader& shader);

		void UpdateData(const std::array<std::array<std::array<Voxel, 32>, 32>, 32>* data, const std::vector<int>& changedMaterialsIn);

		[[nodiscard]] glm::mat4x4 GetTransform() const;

		[[nodiscard]] unsigned int GetMeshId() const;

	    [[nodiscard]] unsigned int GetVertexCount() const;

	private:
		bool dirty = false;

		glm::mat4x4 transform;

	    std::vector<int> changedMaterials;

		unsigned int voxelDataSsbo, voxelMeshSsbo, meshCounter;

	    unsigned int vertexCount;
	};
}
