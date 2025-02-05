#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include <vector>

#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>

namespace tinygltf
{
	class Animation;
	class AnimationChannel;
	class Model;
}

namespace Vox
{
	class AnimationChannel
	{
	public:
		enum class SamplerType : char
		{
			Translation,
			Rotation,
			Scale,
			Error
		};

		AnimationChannel(tinygltf::Model& model, const tinygltf::Animation& animation, const tinygltf::AnimationChannel& channel);

		glm::quat EvaluateRotation(float time) const;
		glm::vec3 EvaulateVector(float time) const;

		static SamplerType GetSamplerType(std::string string);

	private:
		int GetLeftIndex(float time) const;

		SamplerType type;

		std::vector<float> timeKeys;

		std::vector<glm::quat> rotations;
		std::vector<glm::vec3> vectors;
	};
}