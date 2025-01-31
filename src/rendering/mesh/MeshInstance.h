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
		MeshInstance();

		void SetTransform(glm::mat4x4 transformIn);

		glm::mat4x4 GetTransform() const;

	private:
		glm::mat4x4 transform;
	};
}