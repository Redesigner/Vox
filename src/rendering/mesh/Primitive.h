#pragma once

#include <tiny_gltf.h>

#include <glm/mat4x4.hpp>

namespace Vox
{
	class Primitive
	{
	public:
		Primitive(unsigned int vertexCount, unsigned int componentType, int materialIndex,
			unsigned int indexBuffer, unsigned int positionBuffer, unsigned int normalBuffer, unsigned int uvBuffer);

		[[nodiscard]] unsigned int GetVertexCount() const;

		[[nodiscard]] unsigned int GetComponentType() const;

		[[nodiscard]] unsigned int GetMaterialIndex() const;

		[[nodiscard]] unsigned int GetIndexBuffer() const;

		[[nodiscard]] unsigned int GetPositionBuffer() const;

		[[nodiscard]] unsigned int GetNormalBuffer() const;

		[[nodiscard]] unsigned int GetUVBuffer() const;

		void SetTransform(glm::mat4x4 transform);

		[[nodiscard]] glm::mat4x4 GetTransform() const;

	private:
		unsigned int vertexCount;
		unsigned int componentType;
		unsigned int materialIndex;
		unsigned int indexBuffer, positionBuffer, normalBuffer, uvBuffer;
		glm::mat4x4 transform;
	};
}