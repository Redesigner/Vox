#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/mat4x4.hpp>

#include "Vox.h"
#include "rendering/mesh/ModelNode.h"
#include "rendering/mesh/Primitive.h"
#include "rendering/PBRMaterial.h"

namespace tinygltf
{
	class Model;
	class Node;
}

namespace Vox
{
	class MaterialShader;
	class Shader;
    class MeshShader;
    class PickShader;

	class Model
	{
	public:
		explicit Model(const std::string& filepath);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(Model&&) = delete;

		void Render(const MaterialShader* shader, const glm::mat4x4& rootMatrix, const std::vector<PBRMaterial>& materialInstances) const;

        void Render(const MeshShader* shader, const glm::mat4x4& rootMatrix) const;

	    [[nodiscard]] const std::vector<PBRMaterial>& GetMaterials() const;

#ifdef EDITOR
		void Render(const PickShader* shader, unsigned int objectId, const glm::mat4x4& rootMatrix) const;
#endif
		
	private:
        static ModelTransform CalculateNodeTransform(const tinygltf::Node& node);

		void UpdateTransforms(unsigned int node, const glm::mat4x4& transform);

		std::vector<unsigned int> bufferIds;

		// store our mesh map separately, so our meshes can be iterated faster
		std::vector<std::vector<unsigned int>> meshes;

		std::vector<Primitive> primitives;

		std::vector<PBRMaterial> materials;

		std::vector<ModelNode> nodes;
	};
}