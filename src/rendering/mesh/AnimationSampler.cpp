#include "AnimationSampler.h"

#include "core/math/Math.h"

namespace Vox
{
	AnimationSampler::AnimationSampler(std::vector<unsigned char>& times, size_t timesStart, size_t timesEnd,
		std::vector<unsigned char>& samples, size_t samplesStart, size_t samplesEnd,
		SamplerType type)
		:type(type)
	{
		float* timeData = reinterpret_cast<float*>(times.data());
		timeKeys = std::vector<float>(timeData + timesStart, timeData + timesEnd);

		switch (type)
		{
		case SamplerType::Translation: case SamplerType::Scale:
		{
			glm::vec3* sampleData = reinterpret_cast<glm::vec3*>(samples.data());
			vectors = std::vector<glm::vec3>(sampleData + samplesStart, sampleData + samplesEnd);
			break;
		}
		case SamplerType::Rotation:
		{
			glm::quat* sampleData = reinterpret_cast<glm::quat*>(samples.data());
			rotations = std::vector<glm::quat>(sampleData + samplesStart, sampleData + samplesEnd);
			break;
		}
		}
	}

	glm::quat Vox::AnimationSampler::EvaluateRotation(float time) const
	{
		assert(!rotations.empty());
		int leftIndex = GetLeftIndex(time);
		if (leftIndex == -1)
		{
			return rotations[0];
		}

		if (leftIndex == rotations.size() - 1)
		{
			return rotations[rotations.size() - 1];
		}

		float alpha = RemapRange(time, timeKeys[leftIndex], timeKeys[leftIndex + 1], 0.0f, 1.0f);

		return glm::mix(rotations[leftIndex], rotations[leftIndex], alpha);
	}

	glm::vec3 AnimationSampler::EvaulateVector(float time) const
	{
		assert(!rotations.empty());
		int leftIndex = GetLeftIndex(time);
		if (leftIndex == -1)
		{
			return vectors[0];
		}

		if (leftIndex == rotations.size() - 1)
		{
			return vectors[rotations.size() - 1];
		}

		float alpha = RemapRange(time, timeKeys[leftIndex], timeKeys[leftIndex + 1], 0.0f, 1.0f);

		return glm::mix(vectors[leftIndex], vectors[leftIndex], alpha);
	}

	AnimationSampler::SamplerType AnimationSampler::GetSamplerType(std::string string)
	{
		if (string == "translation")
		{
			return SamplerType::Translation;
		}

		if (string == "rotation")
		{
			return SamplerType::Rotation;
		}

		if (string == "scale")
		{
			return SamplerType::Scale;
		}

		return SamplerType::Error;
	}

	int AnimationSampler::GetLeftIndex(float time) const
	{
		// Binary search
		unsigned int left = 0;
		unsigned int right = timeKeys.size();
		unsigned int current = right / 2;

		for (;;)
		{
			if (left + 1 == right)
			{
				return left;
			}

			const float comparedTime = timeKeys[current];
			if (time == comparedTime)
			{
				return current;
			}

			if (time < comparedTime)
			{
				int newRight = current;
				current = (left + right) / 2;
				right = newRight;
			}
			else
			{
				int newLeft = current;
				current = (left + right) / 2;
				left = newLeft;
			}
		}
	}
}