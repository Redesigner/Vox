#pragma once

#include <string>
#include <vector>

#include <glm/mat4x4.hpp>

#include "core/datatypes/Transform.h"

namespace Vox
{
	struct ModelNode
	{
	public:
		std::string name;

		std::vector<int> children;

		int mesh = -1;

		Transform localTransform;
		glm::mat4x4 globalTransform = glm::identity<glm::mat4x4>();

		// @TODO: make a separate class nodes with inverse bind matrices?
		glm::mat4x4 inverseBindMatrix = glm::identity<glm::mat4x4>();

		bool root = true;
	};
}