#include "Mesh.h"

namespace Vox
{
	Mesh::Mesh(tinygltf::Model& meshSource, unsigned int meshIndex)
	{
		tinygltf::Mesh& mesh = meshSource.meshes.at(meshIndex);

		std::vector<int> requiredBufferViews;
		// Find whichever bufferViews we need

		for (tinygltf::Primitive primitive : mesh.primitives)
		{
			for (const std::pair<std::string, int>& attributes : primitive.attributes)
			{
				requiredBufferViews.emplace_back(attributes.second);
			}
		}

		std::vector<int> requiredBuffers;
		for (int bufferView : requiredBufferViews)
		{
			meshSource.bufferViews[bufferView];
		}
	}
}
