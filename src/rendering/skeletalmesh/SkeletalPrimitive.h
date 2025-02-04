#pragma once

namespace Vox
{
	// A primitive with additiional buffers for joints and weights
	struct SkeletalPrimitive
	{
		unsigned int vertexCount;
		unsigned int componentType;
		unsigned int materialIndex;
		unsigned int indexBuffer, positionBuffer, normalBuffer, uvBuffer, jointsBuffer, weightsBuffer;
	};
}