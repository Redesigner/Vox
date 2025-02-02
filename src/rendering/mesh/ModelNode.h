#pragma once

#include <vector>

#include "core/datatypes/Transform.h"

namespace Vox
{
	class Mesh;
	class ModelNode
	{
	public:
		ModelNode();

		void UpdateTransform(Transform newTransform);

	private:
		std::vector<ModelNode*> children;
		std::vector<Mesh*> meshes;

		Transform localTransform;
		Transform globalTransform;

		bool root = false;
	};
}