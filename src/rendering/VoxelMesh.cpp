#include "VoxelMesh.h"

#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>

namespace Vox
{
	VoxelMesh::VoxelMesh(glm::ivec2 position)
	{
		unsigned int buffers[3] = { 0 };
		glGenBuffers(3, buffers);
		voxelDataSsbo = buffers[0];
		voxelMeshSsbo = buffers[1];
		meshCounter = buffers[2];

		transform = glm::translate(glm::mat4x4(1.0f), glm::vec3(position.x, 0, position.y));
	}

	VoxelMesh::~VoxelMesh()
	{
		unsigned int buffers[2] = { voxelDataSsbo, voxelMeshSsbo };
		glDeleteBuffers(2, buffers);
	}

	bool VoxelMesh::NeedsRegeneration() const
	{
		return dirty;
	}

	void VoxelMesh::Regenerate()
	{
		unsigned int counterStart = 0;
		glNamedBufferData(meshCounter, sizeof(unsigned int), &counterStart, GL_STREAM_READ);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, voxelDataSsbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, voxelMeshSsbo);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, meshCounter);

		glDispatchCompute(1, 1, 1);
		dirty = false;
	}

	void VoxelMesh::UpdateData(std::array<std::array<std::array<Voxel, 32>, 32>, 32>* data)
	{
		glNamedBufferData(voxelMeshSsbo, sizeof(data), data, GL_DYNAMIC_DRAW);
		dirty = true;
	}

	glm::mat4x4 VoxelMesh::GetTransform()
	{
		return transform;
	}

	unsigned int VoxelMesh::GetMeshId() const
	{
		return voxelMeshSsbo;
	}

	VoxelMeshRef::VoxelMeshRef()
		:container(nullptr), index(0)
	{
	}

	VoxelMeshRef::VoxelMeshRef(std::vector<VoxelMesh>* container, size_t index)
		:container(container), index(index)
	{
	}

	VoxelMesh* VoxelMeshRef::operator->()
	{
		assert(container);
		return &container->at(index);
	}

	VoxelMesh& VoxelMeshRef::operator*()
	{
		assert(container);
		return container->at(index);
	}

	const VoxelMesh& VoxelMeshRef::operator*() const
	{
		assert(container);
		return container->at(index);
	}
}