#pragma once

#include <memory>
#include <string>
#include "core/datatypes/ObjectContainer.h"
#include "core/datatypes/Ref.h"
#include "rendering/mesh/MeshInstance.h"
#include "rendering/mesh/Model.h"

namespace Vox
{
	class Model;
	class Shader;

	class MeshInstanceContainer
	{
	public:
		explicit MeshInstanceContainer(size_t size);

		bool LoadMesh(const std::string& filepath);

		void Render(Shader& shader, unsigned int modelUniformLocation, unsigned int colorUniformLocation, unsigned int roughnessUniformLocation);

		Ref<MeshInstance> CreateMeshInstance();

		[[nodiscard]] size_t GetInstanceCount() const;

	private:
		std::unique_ptr<Model> model;
		ObjectContainer<MeshInstance> meshInstances;
	};
}