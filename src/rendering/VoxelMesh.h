#pragma once

#include <array>
#include <vector>

#include <glm/glm.hpp>

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

	// @TODO: make a templated version of this class for vectors?
	// What happens if our reference is invalidated?
	struct VoxelMeshRef
	{
	public:
		VoxelMeshRef();

		VoxelMeshRef(std::vector<VoxelMesh>* container, size_t index);

		VoxelMesh* operator->();

		VoxelMesh& operator*();

		const VoxelMesh& operator*() const;

	private:
		std::vector<VoxelMesh>* container;
		size_t index;
	};
}