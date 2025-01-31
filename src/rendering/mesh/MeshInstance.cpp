#include "MeshInstance.h"

#include <glm/ext/matrix_transform.hpp>

namespace Vox
{
    MeshInstance::MeshInstance()
    {
        transform = glm::identity<glm::mat4x4>();
    }

    void MeshInstance::SetTransform(glm::mat4x4 transformIn)
    {
        transform = transformIn;
    }

    glm::mat4x4 MeshInstance::GetTransform() const
    {
        return transform;
    }
}