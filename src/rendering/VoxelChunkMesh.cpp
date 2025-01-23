#include "VoxelChunkMesh.h"

#include <GL/glew.h>

#include "rendering/shaders/ComputeShader.h"

namespace Vox
{
	VoxelChunkMesh::VoxelChunkMesh()
	{
		unsigned int buffers[3] = { 0 };
		glGenBuffers(3, buffers);
		voxelDataSsbo = buffers[0];
		voxelMeshSsbo = buffers[1];
		meshCounter = buffers[2];
	}

	VoxelChunkMesh::~VoxelChunkMesh()
	{
		unsigned int buffers[2] = { voxelDataSsbo, voxelMeshSsbo };
		glDeleteBuffers(2, buffers);
	}

	bool VoxelChunkMesh::NeedsRegeneration() const
	{
		return dirty;
	}

	void VoxelChunkMesh::Regenerate(ComputeShader& shader)
	{
		shader.Enable();
		unsigned int counterStart = 0;
		glNamedBufferData(meshCounter, sizeof(unsigned int), &counterStart, GL_STREAM_READ);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, voxelDataSsbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, voxelMeshSsbo);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, meshCounter);

		glDispatchCompute(1, 1, 1);
	}

	void VoxelChunkMesh::UpdateData(std::array<std::array<std::array<Voxel, 32>, 32>, 32>* data)
	{
		glNamedBufferData(voxelMeshSsbo, sizeof(data), data, GL_DYNAMIC_DRAW);
		dirty = true;
	}
}