#include "Transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

namespace Vox
{
	Transform::Transform()
		:position(0.0f), rotation(0.0f), scale(1.0f)
	{
	}

	Transform::Transform(const glm::mat4x4& matrix)
	    :position({0.0f}), rotation({0.0f}), scale({1.0f})
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::quat rotationQuat;
		decompose(matrix, scale, rotationQuat, position, skew, perspective);
		rotation = glm::eulerAngles(rotationQuat);
	}

    Transform::Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
        :position(position), rotation(rotation), scale(scale)
    {
    }

    bool Transform::operator==(const Transform& other) const
    {
        return position == other.position && rotation == other.rotation && scale == other.scale;
    };

    glm::vec3 Transform::GetForwardVector() const
    {
        return glm::toMat4(glm::quat(rotation)) * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    }

	glm::mat4x4 Transform::GetMatrix() const
	{
		return glm::scale(
		    glm::translate(glm::identity<glm::mat4x4>(), position) * glm::toMat4(glm::quat(rotation)),
		    scale);
	}

	ModelTransform::ModelTransform()
		:position(glm::vec3(0.0f, 0.0f, 0.0f)), rotation(glm::quat(0.0f, 0.0f, 0.0f, 0.0f)), scale(glm::vec3(1.0f, 1.0f, 1.0f))
	{
	}

	ModelTransform::ModelTransform(const glm::mat4x4& matrix)
        :position({0.0f}), rotation(), scale({1.0f})
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		// discard the skew and perspective
		decompose(matrix, scale, rotation, position, skew, perspective);
	}

	glm::mat4x4 ModelTransform::GetMatrix() const
	{
		return glm::scale(glm::translate(glm::identity<glm::mat4x4>(), position) * glm::toMat4(rotation), scale);
	}
}