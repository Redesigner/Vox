#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <glm/mat4x4.hpp>

#include "rendering/PBRMaterial.h"
#include "rendering/mesh/ModelNode.h"
#include "rendering/skeletal_mesh/Animation.h"
#include "rendering/skeletal_mesh/AnimationChannel.h"
#include "rendering/skeletal_mesh/SkeletalPrimitive.h"

namespace tinygltf
{
	class Model;
	class Node;
}

namespace Vox
{
    class GBufferShader;
	class Shader;

	class SkeletalModel
	{
	public:
		explicit SkeletalModel(const std::string& filepath);
		~SkeletalModel();

		SkeletalModel(const SkeletalModel&) = delete;
		SkeletalModel& operator=(SkeletalModel&&) = delete;

		void Render(GBufferShader* shader, glm::mat4x4 transform);

		// @TODO: use something faster than string here
		void SetAnimation(std::string animationName , float time);

		[[nodiscard]] const std::unordered_map<std::string, Animation>& GetAnimations() const;

	private:
		[[nodiscard]] ModelTransform CalculateNodeTransform(const tinygltf::Node& node) const;

		void UpdateTransforms(unsigned int node, glm::mat4x4 transform);

		[[nodiscard]] std::vector<unsigned int> GetMeshBuffers(const tinygltf::Model& model) const;

		std::vector<unsigned int> bufferIds;

		unsigned int matrixBuffer;

		// store our mesh map separately, so our meshes can be iterated faster
		std::vector<std::vector<unsigned int>> meshes;

		std::vector<SkeletalPrimitive> primitives;

		std::vector<PBRMaterial> materials;

		std::unordered_map<std::string, Animation> animations;

		std::vector<ModelNode> nodes;
		std::vector<unsigned int> rootNodes;

		std::vector<std::vector<glm::mat4x4>> skins;

		std::vector<int> joints;

		static constexpr unsigned int maxMatrixCount = 64;

		float currentAnimTime = 0.0f;
	};
}