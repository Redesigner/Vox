#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Vox
{
	Transform::Transform()
		:position(glm::vec3(0.0f, 0.0f, 0.0f)), rotation(glm::quat(0.0f, 0.0f, 0.0f, 0.0f)), scale(glm::vec3(1.0f, 1.0f, 1.0f))
	{
	}

	Transform::Transform(glm::mat4x4 matrix)
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		// discard the skew and perspective
		glm::decompose(matrix, scale, rotation, position, skew, perspective);
	}

	glm::mat4x4 Transform::GetMatrix() const
	{
		return glm::scale(glm::translate(glm::identity<glm::mat4x4>(), position) * glm::toMat4(rotation), scale);
	}
}