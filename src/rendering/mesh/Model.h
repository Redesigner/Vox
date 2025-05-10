#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/mat4x4.hpp>

#include "Vox.h"
#include "rendering/mesh/ModelNode.h"
#include "rendering/mesh/Primitive.h"
#include "rendering/PBRMaterial.h"

namespace Vox
{
    class MeshShader;
    class PickShader;
}

namespace Vox
{
	class GBufferShader;
}

namespace tinygltf
{
	class Model;
	class Node;
}

namespace Vox
{
	class Shader;

	class Model
	{
	public:
		Model(const std::string& filepath);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(Model&&) = delete;

		void Render(const GBufferShader* shader, const glm::mat4x4& rootMatrix);

        void Render(const MeshShader* shader, const glm::mat4x4& rootMatrix) const;

#ifdef EDITOR
		void Render(const PickShader* shader, int objectId, const glm::mat4x4& rootMatrix);
#endif
		
	private:
		ModelTransform CalculateNodeTransform(const tinygltf::Node& node) const;

		void UpdateTransforms(unsigned int node, glm::mat4x4 transform);

		std::vector<unsigned int> bufferIds;

		// store our mesh map separately, so our meshes can be iterated faster
		std::vector<std::vector<unsigned int>> meshes;

		std::vector<Primitive> primitives;

		std::vector<PBRMaterial> materials;

		std::vector<ModelNode> nodes;
	};
}