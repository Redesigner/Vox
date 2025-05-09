#include "MeshInstanceContainer.h"

#include "core/logging/Logging.h"
#include "rendering/shaders/Shader.h"

namespace Vox
{
	MeshInstanceContainer::MeshInstanceContainer(size_t size)
		:meshInstances(size)
	{
	}

	bool MeshInstanceContainer::LoadMesh(const std::string& filepath)
	{
		if (model)
		{
			VoxLog(Error, Rendering, "Failed to load mesh to MeshInstanceContainer - the container already has a mesh loaded.");
			return false;
		}

		model = std::make_unique<Model>(filepath);
		return true;
	}

	void MeshInstanceContainer::Render(const GBufferShader* shader)
	{
		for (std::optional<MeshInstance>& meshInstance : meshInstances)
		{
			if (meshInstance.has_value())
			{
				model->Render(shader, meshInstance->GetTransform());
			}
		}
	}

#ifdef EDITOR
	void MeshInstanceContainer::Render(const PickShader* shader)
	{
		for (std::optional<MeshInstance>& meshInstance : meshInstances)
		{
			if (meshInstance.has_value())
			{
				model->Render(shader, meshInstance->GetPickId(), meshInstance->GetTransform());
			}
		}
	}
#endif

	Ref<MeshInstance> MeshInstanceContainer::CreateMeshInstance()
	{
		return {&meshInstances, meshInstances.Create(this)};
	}

	size_t MeshInstanceContainer::GetInstanceCount() const
	{
		return meshInstances.size();
	}
}