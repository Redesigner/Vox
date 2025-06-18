#include "VoxelMesh.h"

#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>

#include "core/services/ServiceLocator.h"
#include "rendering/Renderer.h"
#include "rendering/shaders/compute_shaders/VoxelGenerationShader.h"
#include "voxel/VoxelChunk.h"

namespace Vox
{
	VoxelMesh::VoxelMesh(const glm::ivec2 position)
	    :transform(glm::translate(glm::mat4x4(1.0f), VoxelChunk::CalculatePosition(position))), vertexCount(0)
	{
		unsigned int buffers[3] = {};
		glCreateBuffers(3, buffers);
		voxelDataSsbo = buffers[0];
		voxelMeshSsbo = buffers[1];
		meshCounter = buffers[2];

        constexpr unsigned int maxVertexCount = 1024 * 16 * 6;
		glNamedBufferStorage(voxelDataSsbo, sizeof(int) * 32 * 32 * 32, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		glNamedBufferStorage(voxelMeshSsbo, sizeof(float) * 16 * maxVertexCount, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		glNamedBufferStorage(meshCounter, sizeof(unsigned int), nullptr,
		    GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	}

	VoxelMesh::~VoxelMesh()
	{
		const unsigned int buffers[3] = { voxelDataSsbo, voxelMeshSsbo, meshCounter };
		glDeleteBuffers(3, buffers);
	}

    VoxelMesh::VoxelMesh(VoxelMesh&& other) noexcept
        :transform(other.transform), vertexCount(other.vertexCount)
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
	    vertexCount = other.vertexCount;

	    other.voxelDataSsbo = other.voxelMeshSsbo = other.meshCounter = 0;
	    return *this;
    }

    bool VoxelMesh::NeedsRegeneration() const
	{
		return dirty;
	}

	void VoxelMesh::Regenerate(VoxelGenerationShader& shader)
	{
        constexpr unsigned int counterStart = 0;
		//glNamedBufferData(meshCounter, sizeof(unsigned int), &counterStart, GL_STREAM_READ);
		glNamedBufferSubData(meshCounter, 0, sizeof(unsigned int), &counterStart);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, voxelDataSsbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, voxelMeshSsbo);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, meshCounter);


	    const Renderer* renderer = ServiceLocator::GetRenderer();
	    //@TODO: cache what materials are contained in this chunk so we don't run the shader for every voxel and material
	    // for (const int materialId : changedMaterials)
	    for (int materialId = 1; materialId < 5; ++materialId)
	    {
            if (const VoxelMaterial* voxelMaterial = renderer->GetVoxelMaterial(materialId))
	        {
	            shader.SetVoxelMaterial(materialId, *voxelMaterial);
		        glDispatchCompute(1, 1, 1);
	        }
	    }
		dirty = false;

	    int quadCount = 0;
	    glGetNamedBufferSubData(meshCounter, 0, sizeof(unsigned int), &quadCount);
	    vertexCount = quadCount * 6;
	}

    void VoxelMesh::UpdateData(const std::array<std::array<std::array<Voxel, 32>, 32>, 32>* data,
        const std::vector<int>& changedMaterialsIn)
	{
		glNamedBufferSubData(voxelDataSsbo, 0, sizeof(*data), data);
	    changedMaterials = changedMaterialsIn;
		dirty = true;
	}

	glm::mat4x4 VoxelMesh::GetTransform() const
    {
		return transform;
	}

	unsigned int VoxelMesh::GetMeshId() const
	{
		return voxelMeshSsbo;
	}

    unsigned int VoxelMesh::GetVertexCount() const
    {
        return vertexCount;
    }
}
