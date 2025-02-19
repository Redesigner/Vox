#pragma once

#include <tiny_gltf.h>

#include <glm/mat4x4.hpp>

namespace Vox
{
	class Primitive
	{
	public:
		Primitive(unsigned int vertexCount, unsigned int componentType, unsigned int materialIndex,
			unsigned int indexBuffer, unsigned int positionBuffer, unsigned int normalBuffer, unsigned int uvBuffer);

		unsigned int GetVertexCount() const;

		unsigned int GetComponentType() const;

		unsigned int GetMaterialIndex() const;

		unsigned int GetIndexBuffer() const;

		unsigned int GetPositionBuffer() const;

		unsigned int GetNormalBuffer() const;

		unsigned int GetUVBuffer() const;

		void SetTransform(glm::mat4x4 transform);

		glm::mat4x4 GetTransform() const;

	private:
		unsigned int vertexCount;
		unsigned int componentType;
		unsigned int materialIndex;
		unsigned int indexBuffer, positionBuffer, normalBuffer, uvBuffer;
		glm::mat4x4 transform;
	};
}