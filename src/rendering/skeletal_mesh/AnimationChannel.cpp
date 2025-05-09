#include "AnimationChannel.h"

#include <iterator>

#include <tiny_gltf.h>

#include "core/math/Math.h"
#include "rendering/mesh/ModelNode.h"

namespace Vox
{
	AnimationChannel::AnimationChannel(tinygltf::Model& model, const tinygltf::Animation& animation, const tinygltf::AnimationChannel& channel)
	{
		const tinygltf::AnimationSampler& sampler = animation.samplers[channel.sampler];
		const tinygltf::BufferView& timeBufferView = model.bufferViews[sampler.input];
		const tinygltf::BufferView& keyframeBufferView = model.bufferViews[sampler.output];

		node = channel.target_node;
		type = GetSamplerTypeFromString(channel.target_path);

		tinygltf::Buffer& timeBuffer = model.buffers[timeBufferView.buffer];
		tinygltf::Buffer& keyframeBuffer = model.buffers[keyframeBufferView.buffer];

		timeKeys = std::vector<float>();
		std::copy(
			reinterpret_cast<float*>(&timeBuffer.data[timeBufferView.byteOffset]),
			reinterpret_cast<float*>(&timeBuffer.data[timeBufferView.byteOffset + timeBufferView.byteLength]),
			std::back_inserter(timeKeys)
		);

		switch (type)
		{
		case SamplerType::Translation: case SamplerType::Scale:
		{
			vectors = std::vector<glm::vec3>();
			std::copy(
				reinterpret_cast<glm::vec3*>(keyframeBuffer.data.data() + keyframeBufferView.byteOffset),
				reinterpret_cast<glm::vec3*>(keyframeBuffer.data.data() + keyframeBufferView.byteOffset + keyframeBufferView.byteLength),
				std::back_inserter(vectors)
			);
			break;
		}
		case SamplerType::Rotation:
		{
			rotations = std::vector<glm::quat>();
			std::copy(
				reinterpret_cast<glm::quat*>(keyframeBuffer.data.data() + keyframeBufferView.byteOffset),
				reinterpret_cast<glm::quat*>(keyframeBuffer.data.data() + keyframeBufferView.byteOffset + keyframeBufferView.byteLength),
				std::back_inserter(rotations)
			);
			break;
		}
		}
	}

	glm::quat Vox::AnimationChannel::EvaluateRotation(float time) const
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

		return glm::slerp(rotations[leftIndex], rotations[leftIndex + 1], alpha);
	}

	glm::vec3 AnimationChannel::EvaulateVector(float time) const
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

	float AnimationChannel::GetDuration() const
	{
		return timeKeys[timeKeys.size() - 1];
	}

	AnimationChannel::SamplerType AnimationChannel::GetType() const
	{
		return type;
	}

	void AnimationChannel::ApplyToNode(std::vector<ModelNode>& nodes, float time)
	{
		ModelNode& targetNode = nodes[node];
		switch (type)
		{
		case SamplerType::Translation:
		{
			targetNode.localTransform.position = EvaulateVector(time);
			break;
		}
		case SamplerType::Rotation:
		{
			targetNode.localTransform.rotation = EvaluateRotation(time);
			break;
		}
		case SamplerType::Scale:
		{
			targetNode.localTransform.scale = EvaulateVector(time);
		}
		}
	}

	AnimationChannel::SamplerType AnimationChannel::GetSamplerTypeFromString(std::string string)
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

	int AnimationChannel::GetLeftIndex(float time) const
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