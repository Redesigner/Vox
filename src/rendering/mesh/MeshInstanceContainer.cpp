#include "MeshInstanceContainer.h"

#include <algorithm>

#include "rendering/SceneRenderer.h"
#include "rendering/shaders/Shader.h"

namespace Vox
{
    MeshInstanceContainer::MeshInstanceContainer(SceneRenderer* owner, const size_t size, const std::shared_ptr<Model>& mesh)
        :owner(owner), mesh(mesh), meshInstances(size)
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

    SceneRenderer* MeshInstanceContainer::GetOwner() const
    {
        return owner;
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
