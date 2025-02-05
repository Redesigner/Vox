#include "Animation.h"

#include <algorithm>

#include <tiny_gltf.h>

#include "rendering/mesh/ModelNode.h"

namespace Vox
{
	Animation::Animation(const tinygltf::Animation& animation, tinygltf::Model& model)
	{
		duration = 0.0f;
		for (const tinygltf::AnimationChannel& channel : animation.channels)
		{
			AnimationChannel& animationChannel = channels.emplace_back(model, animation, channel);
			duration = std::max(duration, animationChannel.GetDuration());
		}
	}

	float Animation::GetDuration() const
	{
		return duration;
	}

	void Animation::ApplyToNodes(std::vector<ModelNode>& nodes, float time)
	{
		for (AnimationChannel& animationChannel : channels)
		{
			animationChannel.ApplyToNode(nodes, time);
		}
	}
}