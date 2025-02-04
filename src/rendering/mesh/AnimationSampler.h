#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <string>
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
			Scale,
			Error
		};

		AnimationSampler(std::vector<unsigned char>& times, size_t timesStart, size_t timesEnd,
			std::vector<unsigned char>& samples, size_t samplesStart, size_t samplesEnd,
			SamplerType type);

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