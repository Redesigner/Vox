#include "PickShader.h"

#include "rendering/Camera.h"

namespace Vox
{
    PickShader::PickShader(const std::string& vertLocation, const std::string& fragLocation)
        :MeshShader(vertLocation, fragLocation)
    {
        uniformLocations.objectId = GetUniformLocation("objectId");
    }

    void PickShader::SetObjectId(const unsigned int objectId) const
    {
        SetUniformUint(uniformLocations.objectId, objectId);
    }
}
