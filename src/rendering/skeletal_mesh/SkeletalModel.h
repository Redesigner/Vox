#pragma once

#include <string>
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
	class Shader;
    class MaterialShader;
    class MeshShader;
    class PickShader;

	class SkeletalModel
	{
	public:
		explicit SkeletalModel(const std::string& filepath);
		~SkeletalModel();

		SkeletalModel(const SkeletalModel&) = delete;
		SkeletalModel& operator=(SkeletalModel&&) = delete;

		void Render(const MaterialShader* shader, const glm::mat4x4& transform, unsigned int animationIndex, float animationTime);

		void Render(const MeshShader* shader, const glm::mat4x4& transform, unsigned int animationIndex, float animationTime);

#ifdef EDITOR
	    void Render(const PickShader* shader, const glm::mat4x4& transform, unsigned int objectId, unsigned int animationIndex, float animationTime);
#endif

		void SetAnimation(unsigned int animationIndex, float time);

	    bool GetAnimationIndex(const std::string& animationName, unsigned int& animationIndexOut) const;

		[[nodiscard]] const std::vector<Animation>& GetAnimations() const;

	private:
		[[nodiscard]] static ModelTransform CalculateNodeTransform(const tinygltf::Node& node);

		void UpdateTransforms(unsigned int node, const glm::mat4x4& transform);

		[[nodiscard]] static std::vector<unsigned int> GetMeshBuffers(const tinygltf::Model& model);

		std::vector<unsigned int> bufferIds;

		unsigned int matrixBuffer;

		// store our mesh map separately, so our meshes can be iterated faster
		std::vector<std::vector<unsigned int>> meshes;

		std::vector<SkeletalPrimitive> primitives;

		std::vector<PBRMaterial> materials;

		std::vector<Animation> animations;

		std::vector<ModelNode> nodes;
		std::vector<unsigned int> rootNodes;

		std::vector<std::vector<glm::mat4x4>> skins;

		std::vector<int> joints;

		static constexpr unsigned int maxMatrixCount = 64;
	};
}