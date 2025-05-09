﻿#include "PickShader.h"

#include "rendering/Camera.h"

namespace Vox
{
    PickShader::PickShader()
        :MeshShader(vertLocation, fragLocation)
    {
        uniformLocations.objectId = GetUniformLocation("objectId");
    }

    void PickShader::SetObjectId(int objectId) const
    {
        SetUniformUint(uniformLocations.objectId, objectId);
    }
}
