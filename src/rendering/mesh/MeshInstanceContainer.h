#pragma once

#include <memory>
#include <string>
#include "Vox.h"
#include "core/datatypes/ObjectContainer.h"
#include "core/datatypes/Ref.h"
#include "rendering/mesh/MeshInstance.h"
#include "rendering/mesh/Model.h"

namespace Vox
{
	class Model;
	class PickShader;
	class Shader;

	class MeshInstanceContainer
	{
	public:
		explicit MeshInstanceContainer(size_t size);

		bool LoadMesh(const std::string& filepath);

		void Render(const GBufferShader* shader);

#ifdef EDITOR
		void Render(const PickShader* shader);
#endif

		Ref<MeshInstance> CreateMeshInstance();

		[[nodiscard]] size_t GetInstanceCount() const;

	private:
		std::unique_ptr<Model> model;
		ObjectContainer<MeshInstance> meshInstances;
	};
}
