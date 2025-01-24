#include "VoxelMesh.h"

#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>

namespace Vox
{
	VoxelMesh::VoxelMesh(glm::ivec2 position)
	{
		unsigned int buffers[3] = { 0 };
		glCreateBuffers(3, buffers);
		voxelDataSsbo = buffers[0];
		voxelMeshSsbo = buffers[1];
		meshCounter = buffers[2];

		const unsigned int maxVertexCount = 1024 * 16 * 6;
		glNamedBufferStorage(voxelDataSsbo, sizeof(int) * 32 * 32 * 32, NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		glNamedBufferStorage(voxelMeshSsbo, sizeof(float) * 16 * maxVertexCount, NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		transform = glm::translate(glm::mat4x4(1.0f), glm::vec3(position.x - 16, -16, position.y - 16));
	}

	VoxelMesh::~VoxelMesh()
	{
		unsigned int buffers[3] = { voxelDataSsbo, voxelMeshSsbo, meshCounter };
		glDeleteBuffers(3, buffers);
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
		glNamedBufferSubData(voxelDataSsbo, 0, sizeof(*data), data);
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