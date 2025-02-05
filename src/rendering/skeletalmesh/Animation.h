#pragma once

#include <vector>

#include "rendering/skeletalmesh/AnimationChannel.h"

namespace tinygltf
{
	class Animation;
	class Model;
}

namespace Vox
{
	class Animation
	{
	public:
		Animation(const tinygltf::Animation& animation, tinygltf::Model& model);

	private:
		std::vector<AnimationChannel> channels;
	};
}