#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Vox
{
	struct Transform
	{
		Transform();
		explicit  Transform(const glm::mat4x4& matrix);

	    bool operator ==(const Transform& other) const;

		glm ::vec3 position{};
		glm::vec3 rotation{};
		glm::vec3 scale{};

	    [[nodiscard]] glm::vec3 GetForwardVector() const;

		glm::mat4x4 GetMatrix() const;
	};
	
	struct ModelTransform
	{
		ModelTransform();
		explicit ModelTransform(const glm::mat4x4& matrix);

		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;

		glm::mat4x4 GetMatrix() const;
	};

}