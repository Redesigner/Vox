#pragma once

#include <array>

#include "voxel/Voxel.h"

namespace Vox
{
	class ComputeShader;

	class VoxelChunkMesh
	{
	public:
		VoxelChunkMesh();
		~VoxelChunkMesh();

		bool NeedsRegeneration() const;

		void Regenerate(ComputeShader& shader);

		void UpdateData(std::array<std::array<std::array<Voxel, 32>, 32>, 32>* data);

	private:
		bool dirty = false;

		unsigned int voxelDataSsbo, voxelMeshSsbo, meshCounter;
	};
}