//
// Created by steph on 5/14/2025.
//

#include "SkeletalMeshInstanceContainer.h"

#include "SkeletalModel.h"
#include "core/logging/Logging.h"
#include "core/services/FileIOService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    SkeletalMeshInstanceContainer::SkeletalMeshInstanceContainer(SceneRenderer* owner, const size_t size, const std::shared_ptr<SkeletalModel>& mesh)
        :owner(owner), mesh(mesh), meshInstances(size)
    {
    }

    bool SkeletalMeshInstanceContainer::LoadMesh(const std::string& filepath)
    {
        if (mesh)
        {
            VoxLog(Error, Rendering, "Failed to load mesh to SkeletalMeshInstanceContainer - the container already has a mesh loaded.");
            return false;
        }

        mesh = std::make_unique<SkeletalModel>(ServiceLocator::GetFileIoService()->GetAssetPath() + "models/" + filepath);
        return true;
    }

    void SkeletalMeshInstanceContainer::Render(MaterialShader* shader)
    {
        for (std::optional<SkeletalMeshInstance>& meshInstance : meshInstances)
        {
            if (meshInstance.has_value())
            {
                mesh->Render(shader, meshInstance->GetTransform(), meshInstance->GetAnimationIndex(), meshInstance->GetAnimationTime());
            }
        }
    }

    void SkeletalMeshInstanceContainer::RenderInstance(const MeshShader* shader,
        const SkeletalMeshInstance& meshInstance) const
    {
        mesh->Render(shader, meshInstance.GetTransform(), meshInstance.GetAnimationIndex(), meshInstance.GetAnimationTime());
    }

    const std::vector<Animation>& SkeletalMeshInstanceContainer::GetAnimations() const
    {
        return mesh->GetAnimations();
    }

#ifdef EDITOR
    void SkeletalMeshInstanceContainer::Render(const PickShader* shader)
    {
        for (std::optional<SkeletalMeshInstance>& meshInstance : meshInstances)
        {
            if (meshInstance.has_value())
            {
                mesh->Render(shader, meshInstance->GetTransform(), meshInstance->GetPickId(), meshInstance->GetAnimationIndex(), meshInstance->GetAnimationTime());
            }
        }
    }
#endif

    Ref<SkeletalMeshInstance> SkeletalMeshInstanceContainer::CreateMeshInstance()
    {
		return {&meshInstances, meshInstances.Create(this)};
    }

    SceneRenderer* SkeletalMeshInstanceContainer::GetOwner() const
    {
        return owner;
    }
} // Vox