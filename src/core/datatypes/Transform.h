#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Vox
{
	struct Transform
	{
		Transform();
		explicit Transform(const glm::mat4x4& matrix);

		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;

		glm::mat4x4 GetMatrix() const;
	};
}