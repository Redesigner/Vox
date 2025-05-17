#include "MeshInstanceContainer.h"

#include <algorithm>

#include "core/logging/Logging.h"
#include "core/services/FileIOService.h"
#include "core/services/ServiceLocator.h"
#include "rendering/shaders/Shader.h"
#include "rendering/shaders/pixel_shaders/mesh_shaders/MaterialShader.h"

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

		model = std::make_unique<Model>(ServiceLocator::GetFileIoService()->GetAssetPath() + "models/" + filepath);
		return true;
	}

	void MeshInstanceContainer::Render(const MaterialShader* shader)
	{
		for (std::optional<MeshInstance>& meshInstance : meshInstances)
		{
			if (meshInstance.has_value() && meshInstance->visible)
			{
				model->Render(shader, meshInstance->GetTransform(), meshInstance->GetMaterials());
			}
		}
	}

    void MeshInstanceContainer::RenderInstance(const MeshShader* shader, const MeshInstance& meshInstance) const
    {
	    model->Render(shader, meshInstance.GetTransform());
    }

    void MeshInstanceContainer::RenderInstance(const MaterialShader* shader, const MeshInstance& meshInstance) const
    {
	    model->Render(shader, meshInstance.GetTransform(), meshInstance.GetMaterials());
    }

#ifdef EDITOR
	void MeshInstanceContainer::Render(const PickShader* shader)
	{
		for (std::optional<MeshInstance>& meshInstance : meshInstances)
		{
			if (meshInstance.has_value() && meshInstance->visible)
			{
				model->Render(shader, meshInstance->GetPickId(), meshInstance->GetTransform());
			}
		}
	}

    void MeshInstanceContainer::RenderInstance(const PickShader* shader, const MeshInstance& meshInstance) const
    {
	    model->Render(shader, meshInstance.GetPickId(), meshInstance.GetTransform());

    }
#endif

	Ref<MeshInstance> MeshInstanceContainer::CreateMeshInstance()
	{
		return {&meshInstances, meshInstances.Create(this, model->GetMaterials())};
	}

	size_t MeshInstanceContainer::GetInstanceCount() const
	{
		return std::ranges::count_if(meshInstances, [](const std::optional<MeshInstance>& meshInstance)
        {
            return meshInstance.has_value();
        });
	}
}
