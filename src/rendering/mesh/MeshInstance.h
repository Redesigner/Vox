#pragma once

#include <functional>
#include <glm/mat4x4.hpp>

#include "Vox.h"

namespace Vox
{
    class MeshInstanceContainer;
	/**
	 * @brief A struct that contains all the information to draw a separate instance of a mesh
	 */
	struct MeshInstance
	{
	public:
		explicit MeshInstance(MeshInstanceContainer* meshOwner);
		~MeshInstance();

	    MeshInstance(MeshInstance&& other) noexcept;
		
		void SetTransform(glm::mat4x4 transformIn);

		[[nodiscard]] glm::mat4x4 GetTransform() const;

	    [[nodiscard]] MeshInstanceContainer* GetMeshOwner() const;

#ifdef EDITOR
		void RegisterCallback(std::function<void(glm::ivec2)> callback);

		[[nodiscard]] unsigned int GetPickId() const;
#endif

	private:
		glm::mat4x4 transform;

	    MeshInstanceContainer* meshOwner;

#ifdef EDITOR
		unsigned int pickId = 0;
#endif
	};
}