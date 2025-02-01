#include "Mesh.h"

namespace Vox
{
	Mesh::Mesh(unsigned int vertexCount, unsigned int componentType, unsigned int materialIndex,
		unsigned int indexBuffer,unsigned int positionBuffer, unsigned int normalBuffer, unsigned int uvBuffer)
		:vertexCount(vertexCount), componentType(componentType), materialIndex(materialIndex),
			indexBuffer(indexBuffer), positionBuffer(positionBuffer), normalBuffer(normalBuffer), uvBuffer(uvBuffer)
	{
	}

	unsigned int Mesh::GetVertexCount() const
	{
		return vertexCount;
	}

	unsigned int Mesh::GetComponentType() const
	{
		return componentType;
	}

	unsigned int Mesh::GetMaterialIndex() const
	{
		return materialIndex;
	}

	unsigned int Mesh::GetIndexBuffer() const
	{
		return indexBuffer;
	}

	unsigned int Mesh::GetPositionBuffer() const
	{
		return positionBuffer;
	}

	unsigned int Mesh::GetNormalBuffer() const
	{
		return normalBuffer;
	}

	unsigned int Mesh::GetUVBuffer() const
	{
		return uvBuffer;
	}

	void Mesh::SetTransform(glm::mat4x4 transformIn)
	{
		transform = transformIn;
	}

	glm::mat4x4 Mesh::GetTransform() const
	{
		return transform;
	}
}
