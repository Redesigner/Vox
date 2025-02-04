#include "AnimationSampler.h"

#include "core/math/Math.h"

namespace Vox
{
	AnimationSampler::AnimationSampler(std::vector<unsigned char>& times, size_t timesStart, size_t timeBytes,
		std::vector<unsigned char>& samples, size_t samplesStart, size_t sampleBytes,
		SamplerType type)
		:type(type)
	{
		timesCount = timeBytes / sizeof(float);
		timeKeys = new float[timesCount];
		std::memcpy(timeKeys, &times[timesStart], timeBytes);

		switch (type)
		{
		case SamplerType::Translation: case SamplerType::Scale:
		{
			samplesCount = sampleBytes / sizeof(glm::vec3);
			vectors = new glm::vec3[samplesCount];
			rotations = nullptr;
			std::memcpy(vectors, &samples[samplesStart], sampleBytes);
			break;
		}
		case SamplerType::Rotation:
		{
			samplesCount = sampleBytes / sizeof(glm::vec3);
			rotations = new glm::quat[sampleBytes];
			vectors = nullptr;
			std::memcpy(rotations, &samples[samplesStart], sampleBytes);
			break;
		}
		}
	}

	AnimationSampler::~AnimationSampler()
	{
		//delete[] timeKeys;
		//delete[] rotations;
		//delete[] vectors;
	}

	glm::quat Vox::AnimationSampler::EvaluateRotation(float time) const
	{
		assert(samplesCount);
		int leftIndex = GetLeftIndex(time);
		if (leftIndex == -1)
		{
			return rotations[0];
		}

		if (leftIndex == samplesCount - 1)
		{
			return rotations[samplesCount - 1];
		}

		float alpha = RemapRange(time, timeKeys[leftIndex], timeKeys[leftIndex + 1], 0.0f, 1.0f);

		return glm::mix(rotations[leftIndex], rotations[leftIndex], alpha);
	}

	glm::vec3 AnimationSampler::EvaulateVector(float time) const
	{
		assert(samplesCount);
		int leftIndex = GetLeftIndex(time);
		if (leftIndex == -1)
		{
			return vectors[0];
		}

		if (leftIndex == samplesCount - 1)
		{
			return vectors[samplesCount - 1];
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
		unsigned int right = timesCount;
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