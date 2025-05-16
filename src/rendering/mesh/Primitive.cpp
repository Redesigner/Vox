#include "Primitive.h"

#include <glm/ext/matrix_transform.hpp>

namespace Vox
{
	Primitive::Primitive(unsigned int vertexCount, unsigned int componentType, int materialIndex,
		unsigned int indexBuffer,unsigned int positionBuffer, unsigned int normalBuffer, unsigned int uvBuffer)
		:vertexCount(vertexCount), componentType(componentType), materialIndex(materialIndex >= 0 ? materialIndex : 0),
		indexBuffer(indexBuffer), positionBuffer(positionBuffer), normalBuffer(normalBuffer), uvBuffer(uvBuffer),
        transform(glm::identity<glm::mat4x4>())
	{
	}

	unsigned int Primitive::GetVertexCount() const
	{
		return vertexCount;
	}

	unsigned int Primitive::GetComponentType() const
	{
		return componentType;
	}

	unsigned int Primitive::GetMaterialIndex() const
	{
		return materialIndex;
	}

	unsigned int Primitive::GetIndexBuffer() const
	{
		return indexBuffer;
	}

	unsigned int Primitive::GetPositionBuffer() const
	{
		return positionBuffer;
	}

	unsigned int Primitive::GetNormalBuffer() const
	{
		return normalBuffer;
	}

	unsigned int Primitive::GetUVBuffer() const
	{
		return uvBuffer;
	}

	void Primitive::SetTransform(glm::mat4x4 transformIn)
	{
		transform = transformIn;
	}

	glm::mat4x4 Primitive::GetTransform() const
	{
		return transform;
	}
}
