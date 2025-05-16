#pragma once

#include <functional>
#include <glm/mat4x4.hpp>

#include "Vox.h"
#include "rendering/PBRMaterial.h"

namespace Vox
{
    class MeshInstanceContainer;
	/**
	 * @brief A struct that contains all the information to draw a separate instance of a mesh
	 */
	struct MeshInstance
	{
		explicit MeshInstance(MeshInstanceContainer* meshOwner, const std::vector<PBRMaterial>& materials);
		~MeshInstance();

	    MeshInstance(MeshInstance&& other) noexcept;

		[[nodiscard]] glm::mat4x4 GetTransform() const;

	    [[nodiscard]] MeshInstanceContainer* GetMeshOwner() const;

	    [[nodiscard]] const std::vector<PBRMaterial>& GetMaterials() const;

		void SetTransform(glm::mat4x4 transformIn);

	    void SetMaterial(unsigned int index, const PBRMaterial& material);

#ifdef EDITOR
		void RegisterClickCallback(std::function<void(glm::ivec2)> callback);

		[[nodiscard]] unsigned int GetPickId() const;
#endif

	    bool visible;

	private:
		glm::mat4x4 transform;
	    std::vector<PBRMaterial> materials;
	    MeshInstanceContainer* meshOwner;


#ifdef EDITOR
		unsigned int pickId = 0;
#endif
	};
}
