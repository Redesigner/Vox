#pragma once

#include <vector>

#include "core/datatypes/Transform.h"

namespace Vox
{
	class Primitive;
	class ModelNode
	{
	public:
		ModelNode();

		void UpdateTransform(Transform newTransform);

	private:
		std::vector<ModelNode*> children;
		std::vector<Primitive*> primitives;

		Transform localTransform;
		Transform globalTransform;

		bool root = false;
	};
}