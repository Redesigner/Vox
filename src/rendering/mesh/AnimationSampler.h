#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <vector>

#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>

namespace Vox
{

	class AnimationSampler
	{
	public:
		enum class SamplerType : char
		{
			Translation,
			Rotation,
			Scale
		};

		AnimationSampler(std::vector<char>& samples, std::vector<char>& times, SamplerType type);

		glm::quat EvaluateRotation(float time) const;
		glm::vec3 EvaulateVector(float time) const;

	private:
		int GetLeftIndex(float time) const;

		SamplerType type;

		std::vector<float> timeKeys;
		std::vector<glm::quat> rotations;
		std::vector<glm::vec3> vectors;
	};
}