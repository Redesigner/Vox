#include "MeshInstanceContainer.h"

#include <algorithm>

#include "core/logging/Logging.h"
#include "core/services/FileIOService.h"
#include "core/services/ServiceLocator.h"
#include "rendering/shaders/Shader.h"
#include "rendering/shaders/pixel_shaders/mesh_shaders/MaterialShader.h"

namespace Vox
{
    MeshInstanceContainer::MeshInstanceContainer(const size_t size, const std::shared_ptr<Model>& mesh)
        :meshInstances(size), mesh(mesh)
    {
    }

	void MeshInstanceContainer::Render(const MaterialShader* shader)
	{
		for (std::optional<MeshInstance>& meshInstance : meshInstances)
		{
			if (meshInstance.has_value() && meshInstance->visible)
			{
				mesh->Render(shader, meshInstance->GetTransform(), meshInstance->GetMaterials());
			}
		}
	}

    void MeshInstanceContainer::RenderInstance(const MeshShader* shader, const MeshInstance& meshInstance) const
    {
	    mesh->Render(shader, meshInstance.GetTransform());
    }

    void MeshInstanceContainer::RenderInstance(const MaterialShader* shader, const MeshInstance& meshInstance) const
    {
	    mesh->Render(shader, meshInstance.GetTransform(), meshInstance.GetMaterials());
    }

#ifdef EDITOR
	void MeshInstanceContainer::Render(const PickShader* shader)
	{
		for (std::optional<MeshInstance>& meshInstance : meshInstances)
		{
			if (meshInstance.has_value() && meshInstance->visible)
			{
				mesh->Render(shader, meshInstance->GetPickId(), meshInstance->GetTransform());
			}
		}
	}

    void MeshInstanceContainer::RenderInstance(const PickShader* shader, const MeshInstance& meshInstance) const
    {
	    mesh->Render(shader, meshInstance.GetPickId(), meshInstance.GetTransform());

    }
#endif

	Ref<MeshInstance> MeshInstanceContainer::CreateMeshInstance()
	{
		return {&meshInstances, meshInstances.Create(this, mesh->GetMaterials())};
	}

	size_t MeshInstanceContainer::GetInstanceCount() const
	{
		return std::ranges::count_if(meshInstances, [](const std::optional<MeshInstance>& meshInstance)
        {
            return meshInstance.has_value();
        });
	}
}
