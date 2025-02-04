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

		AnimationSampler(std::vector<unsigned char>& times, size_t timesStart, size_t timeBytes,
			std::vector<unsigned char>& samples, size_t samplesStart, size_t sampleBytes,
			SamplerType type);

		~AnimationSampler();

		glm::quat EvaluateRotation(float time) const;
		glm::vec3 EvaulateVector(float time) const;

		static SamplerType GetSamplerType(std::string string);

	private:
		int GetLeftIndex(float time) const;

		SamplerType type;

		float* timeKeys;
		size_t timesCount = 0;

		glm::quat* rotations;
		glm::vec3* vectors;
		size_t samplesCount = 0;
	};
}