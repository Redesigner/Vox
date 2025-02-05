#include "Animation.h"

#include <tiny_gltf.h>

namespace Vox
{
	Animation::Animation(const tinygltf::Animation& animation, tinygltf::Model& model)
	{
		for (const tinygltf::AnimationChannel& channel : animation.channels)
		{
			channels.emplace_back(model, animation, channel);
		}
	}
}