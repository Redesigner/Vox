#include "VoxelMesh.h"

#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>

#include "voxel/VoxelChunk.h"

namespace Vox
{
	VoxelMesh::VoxelMesh(const glm::ivec2 position)
	    :transform(glm::translate(glm::mat4x4(1.0f), VoxelChunk::CalculatePosition(position)))
	{
		unsigned int buffers[3] = {};
		glCreateBuffers(3, buffers);
		voxelDataSsbo = buffers[0];
		voxelMeshSsbo = buffers[1];
		meshCounter = buffers[2];

		const unsigned int maxVertexCount = 1024 * 16 * 6;
		glNamedBufferStorage(voxelDataSsbo, sizeof(int) * 32 * 32 * 32, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		glNamedBufferStorage(voxelMeshSsbo, sizeof(float) * 16 * maxVertexCount, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		glNamedBufferStorage(meshCounter, sizeof(unsigned int), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
	}

	VoxelMesh::~VoxelMesh()
	{
		unsigned int buffers[3] = { voxelDataSsbo, voxelMeshSsbo, meshCounter };
		glDeleteBuffers(3, buffers);
	}

    VoxelMesh::VoxelMesh(VoxelMesh&& other) noexcept
        :transform(other.transform)
    {
	    voxelDataSsbo = other.voxelDataSsbo;
	    voxelMeshSsbo = other.voxelMeshSsbo;
	    meshCounter = other.meshCounter;

	    other.voxelDataSsbo = other.voxelMeshSsbo = other.meshCounter = 0;
    }

    VoxelMesh& VoxelMesh::operator=(VoxelMesh&& other) noexcept
    {
	    voxelDataSsbo = other.voxelDataSsbo;
	    voxelMeshSsbo = other.voxelMeshSsbo;
	    meshCounter = other.meshCounter;

	    transform = other.transform;

	    other.voxelDataSsbo = other.voxelMeshSsbo = other.meshCounter = 0;
	    return *this;
    }

    bool VoxelMesh::NeedsRegeneration() const
	{
		return dirty;
	}

	void VoxelMesh::Regenerate()
	{
		unsigned int counterStart = 0;
		//glNamedBufferData(meshCounter, sizeof(unsigned int), &counterStart, GL_STREAM_READ);
		glNamedBufferSubData(meshCounter, 0, sizeof(unsigned int), &counterStart);
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
}
