#pragma once

#include <glm/mat4x4.hpp>

namespace Vox
{
	/**
	 * @brief A struct that contains all the information to draw a separate instance of a mesh
	 */
	struct MeshInstance
	{
	public:


	private:
		glm::mat4x4 transform;
	};
}