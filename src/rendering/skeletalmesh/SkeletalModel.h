#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/mat4x4.hpp>

#include "rendering/mesh/ModelNode.h"
#include "rendering/skeletalmesh/AnimationSampler.h"
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

	private:
		Transform CalculateNodeTransform(const tinygltf::Node& node) const;

		void UpdateTransforms(unsigned int node, glm::mat4x4 transform);

		std::vector<unsigned int> GetMeshBuffers(const tinygltf::Model& model) const;

		std::vector<std::tuple<unsigned int, unsigned int, AnimationSampler::SamplerType>> GetAnimationBuffers(const tinygltf::Model& model) const;

		std::vector<unsigned int> bufferIds;

		// store our mesh map separately, so our meshes can be iterated faster
		std::vector<std::vector<unsigned int>> meshes;

		std::vector<SkeletalPrimitive> primitives;

		std::vector<PBRMaterial> materials;

		std::vector<ModelNode> nodes;

		std::vector<AnimationSampler> samplers;
	};
}