#include "Animation.h"

#include <algorithm>

#include <tiny_gltf.h>

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
}