//
// Created by steph on 5/23/2025.
//

#include "FlyCamera.h"

#include <algorithm>

#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    FlyCamera::FlyCamera()
    {
        mouseLookDelegate = ServiceLocator::GetInputService()->RegisterMouseMotionCallback([this](int xMotion, int yMotion)
        {
            RotateCamera(xMotion, yMotion);
        });
    }

    FlyCamera::~FlyCamera()
    {
        ServiceLocator::GetInputService()->UnregisterMouseMotionCallback(mouseLookDelegate);
    }

    void FlyCamera::Tick(float deltaTime)
    {
        const InputService* inputService = ServiceLocator::GetInputService();
        const glm::vec3 input = {
            inputService->GetInputAxis({SDL_SCANCODE_A, SDL_SCANCODE_D}),
            inputService->GetInputAxis({SDL_SCANCODE_Q, SDL_SCANCODE_E}),
            inputService->GetInputAxis({SDL_SCANCODE_W, SDL_SCANCODE_S})
            };
        if (input.x == 0 && input.y == 0 && input.z == 0)
        {
            return;
        }

        const glm::vec3 delta =
            GetRightVector() * input.x +
            glm::vec3(0.0f, input.y, 0.0f) +
            GetForwardVector() * input.z;

        SetPosition(GetPosition() + -delta * deltaTime);
    }

    void FlyCamera::RotateCamera(int x, int y)
    {
        glm::vec3 rotation = GetRotation();
        rotation.x += static_cast<float>(y) / 500.0f;
        rotation.x = std::clamp(rotation.x, -1.5f, 1.5f);
        rotation.y += static_cast<float>(x) / 500.0f;
        SetRotation(rotation);
    }
} // Vox