#include "MeshInstanceContainer.h"

#include "core/logging/Logging.h"
#include "rendering/mesh/Model.h"
#include "rendering/shaders/Shader.h"

namespace Vox
{
	MeshInstanceContainer::MeshInstanceContainer(size_t size)
		:meshInstances(size)
	{
	}

	bool MeshInstanceContainer::LoadMesh(std::string filepath)
	{
		if (model)
		{
			VoxLog(Error, Rendering, "Failed to load mesh to MeshInstanceContainer - the container already has a mesh loaded.");
			return false;
		}

		model = std::make_unique<Model>(filepath);
		return true;
	}

	void MeshInstanceContainer::Render(Shader& shader, unsigned int modelUniformLocation, unsigned int colorUniformLocation, unsigned int roughnessUniformLocation)
	{
		model->Render(shader, modelUniformLocation, colorUniformLocation, roughnessUniformLocation);
	}

	Ref<MeshInstance> MeshInstanceContainer::CreateMeshInstance()
	{
		return Ref<MeshInstance>(&meshInstances, meshInstances.Create());
	}
}