#include "AnimationSampler.h"

#include <iterator>

#include "core/math/Math.h"

namespace Vox
{
	AnimationSampler::AnimationSampler(std::vector<unsigned char>& times, size_t timesStart, size_t timeBytes,
		std::vector<unsigned char>& samples, size_t samplesStart, size_t sampleBytes,
		SamplerType type)
		:type(type)
	{
		timeKeys = std::vector<float>();
		std::copy(reinterpret_cast<float*>(&times[timesStart]), reinterpret_cast<float*>(&times[timesStart + timeBytes]), std::back_inserter(timeKeys));

		switch (type)
		{
		case SamplerType::Translation: case SamplerType::Scale:
		{
			vectors = std::vector<glm::vec3>();
			std::copy(reinterpret_cast<glm::vec3*>(&samples[samplesStart]), reinterpret_cast<glm::vec3*>(& samples[samplesStart + sampleBytes]), std::back_inserter(vectors));
			break;
		}
		case SamplerType::Rotation:
		{
			rotations = std::vector<glm::quat>();
			std::copy(reinterpret_cast<glm::quat*>(&samples[samplesStart]), reinterpret_cast<glm::quat*>(&samples[samplesStart + sampleBytes]), std::back_inserter(rotations));
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
			return rotations[timeKeys.size() - 1];
		}

		float alpha = RemapRange(time, timeKeys[leftIndex], timeKeys[leftIndex + 1], 0.0f, 1.0f);

		return glm::mix(rotations[leftIndex], rotations[leftIndex], alpha);
	}

	glm::vec3 AnimationSampler::EvaulateVector(float time) const
	{
		assert(!vectors.empty());
		int leftIndex = GetLeftIndex(time);
		if (leftIndex == -1)
		{
			return vectors[0];
		}

		if (leftIndex == vectors.size() - 1)
		{
			return vectors[vectors.size() - 1];
		}

		float alpha = RemapRange(time, timeKeys[leftIndex], timeKeys[leftIndex + 1], 0.0f, 1.0f);

		return glm::mix(vectors[leftIndex], vectors[leftIndex + 1], alpha);
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

		if (timeKeys.size() == 1)
		{
			return timeKeys.front();
		}

		unsigned int left = 0;
		unsigned int right = timeKeys.size() - 1;
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