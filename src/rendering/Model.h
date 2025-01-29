#pragma once

#include <memory>
#include <string>
#include <vector>

namespace tinygltf
{
	class Model;
}

namespace Vox
{
	class Model
	{
	public:
		Model(std::string filepath);
		~Model();

		void Render();

	private:
		std::vector<unsigned int> bufferIds;

		std::unique_ptr<tinygltf::Model> model;
	};
}