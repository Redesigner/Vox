#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/mat4x4.hpp>

#include "rendering/mesh/ModelNode.h"
#include "rendering/mesh/Primitive.h"
#include "rendering/PBRMaterial.h"

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
		Model(std::string filepath);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(Model&&) = delete;

		void Render(GBufferShader* shader, const glm::mat4x4& rootMatrix);

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