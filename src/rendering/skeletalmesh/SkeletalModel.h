#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/mat4x4.hpp>

#include "rendering/mesh/ModelNode.h"
#include "rendering/skeletalmesh/Animation.h"
#include "rendering/skeletalmesh/AnimationChannel.h"
#include "rendering/skeletalmesh/SkeletalPrimitive.h"
#include "rendering/PBRMaterial.h"

namespace tinygltf
{
	class Model;
	class Node;
}

namespace Vox
{
	class Shader;

	class SkeletalModel
	{
	public:
		SkeletalModel(std::string filepath);
		~SkeletalModel();

		SkeletalModel(const SkeletalModel&) = delete;
		SkeletalModel& operator=(SkeletalModel&&) = delete;

		void Render(Shader& shader, unsigned int modelUniformLocation, glm::mat4x4 transform, unsigned int colorUniformLocation, unsigned int roughnessUniformLocation);

		// @TODO: use something faster than string here
		void SetAnimation(std::string animationName , float time);

		const std::unordered_map<std::string, Animation>& GetAnimations() const;

	private:
		ModelTransform CalculateNodeTransform(const tinygltf::Node& node) const;

		void UpdateTransforms(unsigned int node, glm::mat4x4 transform);

		std::vector<unsigned int> GetMeshBuffers(const tinygltf::Model& model) const;

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

		static const unsigned int maxMatrixCount = 64;

		float currentAnimTime = 0.0f;
	};
}