#pragma once

#include <vector>

#include "rendering/skeletal_mesh/AnimationChannel.h"

namespace tinygltf
{
	class Animation;
	class Model;
}

namespace Vox
{
	struct ModelNode;

	class Animation
	{
	public:
		Animation(const tinygltf::Animation& animation, tinygltf::Model& model);

		float GetDuration() const;

		void ApplyToNodes(std::vector<ModelNode>& nodes, float time);

	private:
		std::vector<AnimationChannel> channels;
		float duration;
	};
}