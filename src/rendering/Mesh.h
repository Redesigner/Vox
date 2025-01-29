#pragma once

#include <tiny_gltf.h>

namespace Vox
{
	// @TODO: consider renaming to primtive to match GLTF naming conventions
	class Mesh
	{
	public:
		Mesh(unsigned int vertexCount, unsigned int componentType, unsigned int indexBuffer, unsigned int positionBuffer, unsigned int normalBuffer, unsigned int uvBuffer);

		unsigned int GetVertexCount() const;

		unsigned int GetComponentType() const;

		unsigned int GetIndexBuffer() const;

		unsigned int GetPositionBuffer() const;

		unsigned int GetNormalBuffer() const;

		unsigned int GetUVBuffer() const;

	private:
		unsigned int vertexCount;
		unsigned int componentType;
		unsigned int indexBuffer, positionBuffer, normalBuffer, uvBuffer;
	};
}