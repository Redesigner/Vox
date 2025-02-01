#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/mat4x4.hpp>

#include "rendering/mesh/Mesh.h"
#include "rendering/PBRMaterial.h"

namespace tinygltf
{
	class Model;
}

namespace Vox
{
	class Shader;

	class Model
	{
	public:
		Model(std::string filepath);
		~Model();

		void Render(Shader& shader, unsigned int modelUniformLocation, glm::mat4x4 transform, unsigned int colorUniformLocation, unsigned int roughnessUniformLocation);

	private:
		std::vector<unsigned int> bufferIds;

		std::vector<Mesh> meshes;

		std::vector<PBRMaterial> materials;
	};
}