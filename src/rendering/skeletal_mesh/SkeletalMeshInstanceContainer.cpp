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
    SkeletalMeshInstanceContainer::SkeletalMeshInstanceContainer(size_t size)
        :meshInstances(size)
    {
    }

    bool SkeletalMeshInstanceContainer::LoadMesh(const std::string& filepath)
    {
        if (model)
        {
            VoxLog(Error, Rendering, "Failed to load mesh to SkeletalMeshInstanceContainer - the container already has a mesh loaded.");
            return false;
        }

        model = std::make_unique<SkeletalModel>(ServiceLocator::GetFileIoService()->GetAssetPath() + "models/" + filepath);
        return true;
    }

    void SkeletalMeshInstanceContainer::Render(GBufferShader* shader)
    {
        for (std::optional<SkeletalMeshInstance>& meshInstance : meshInstances)
        {
            if (meshInstance.has_value())
            {
                model->Render(shader, meshInstance->GetTransform(), meshInstance->GetAnimationIndex(), meshInstance->GetAnimationTime());
            }
        }
    }

    void SkeletalMeshInstanceContainer::RenderInstance(const MeshShader* shader,
        const SkeletalMeshInstance& meshInstance) const
    {
    }

    const std::vector<Animation>& SkeletalMeshInstanceContainer::GetAnimations() const
    {
        return model->GetAnimations();
    }

    Ref<SkeletalMeshInstance> SkeletalMeshInstanceContainer::CreateMeshInstance()
    {
		return {&meshInstances, meshInstances.Create(this)};
    }
} // Vox