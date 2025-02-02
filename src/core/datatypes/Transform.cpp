#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>

namespace Vox
{
	glm::mat4x4 Transform::GetMatrix() const
	{
		return glm::scale(glm::translate(glm::identity<glm::mat4x4>(), position) * glm::toMat4(rotation), scale);
	}
}