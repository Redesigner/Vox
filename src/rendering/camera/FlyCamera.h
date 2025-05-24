//
// Created by steph on 5/23/2025.
//

#pragma once
#include "Camera.h"
#include "core/datatypes/DelegateHandle.h"

namespace Vox
{

    class FlyCamera : public Camera
    {
    public:
        FlyCamera();
        ~FlyCamera();

        void Tick(float deltaTime);

    private:
        void RotateCamera(int x, int y);

        DelegateHandle<int, int> mouseLookDelegate;
    };

} // Vox
