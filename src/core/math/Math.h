#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Vox
{
	float RemapRange(float x, float minIn, float maxIn, float minOut, float maxOut);

	int FloorMultiple(float x, unsigned int multiple);
}