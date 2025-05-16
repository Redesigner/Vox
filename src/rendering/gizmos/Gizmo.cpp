//
// Created by steph on 5/15/2025.
//

#include "Gizmo.h"

#include <glm/gtc/matrix_inverse.inl>

#include "core/logging/Logging.h"
#include "core/services/EditorService.h"
#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"
#include "rendering/Camera.h"
#include "rendering/Renderer.h"

namespace Vox
{
    Gizmo::Gizmo()
        :clickLocation(0, 0),
        red({1.0f, 0.1f, 0.0f, 1.0f}, 0.0f, 0.0f),
        green({0.0f, 1.0f, 0.0f, 1.0f}, 0.0f, 0.0f),
        blue({0.0f, 0.0f, 1.0f, 1.0f}, 0.0f, 0.0f),
        yellow({1.0f, 1.0f, 0.0f, 1.0f}, 0.0f, 0.0f)
    {
        xArrowMesh = ServiceLocator::GetRenderer()->CreateMeshInstance("gizmoArrow");
        yArrowMesh = ServiceLocator::GetRenderer()->CreateMeshInstance("gizmoArrow");
        zArrowMesh = ServiceLocator::GetRenderer()->CreateMeshInstance("gizmoArrow");

        xArrowMesh->SetMaterial(0, red);
        yArrowMesh->SetMaterial(0, green);
        zArrowMesh->SetMaterial(0, blue);

        transform.position = {0.0f, 4.0f, 0.0f};
        SetTransforms();

        mouseReleaseCallback = ServiceLocator::GetInputService()->RegisterMouseReleaseCallback([this]()
        {
            MouseReleased();
        });

        ServiceLocator::GetInputService()->RegisterMouseMotionCallback([this](int x, int y)
        {
            MouseMoved(x, y);
        });

        xArrowMesh->RegisterClickCallback([this](const glm::ivec2 position)
        {
            clickLocation = position;
            xArrowMesh->SetMaterial(0, yellow);
            selectedAxis = XSelected;
            UpdateScreenSpaceVector({1.0f, 0.0f, 0.0f});
        });

        yArrowMesh->RegisterClickCallback([this](const glm::ivec2 position)
        {
            clickLocation = position;
            yArrowMesh->SetMaterial(0, yellow);
            selectedAxis = YSelected;
            UpdateScreenSpaceVector({0.0f, 1.0f, 0.0f});
        });

        zArrowMesh->RegisterClickCallback([this](const glm::ivec2 position)
        {
            clickLocation = position;
            zArrowMesh->SetMaterial(0, yellow);
            selectedAxis = ZSelected;
            UpdateScreenSpaceVector({0.0f, 0.0f, 1.0f});
        });
    }

    void Gizmo::Update()
    {
        if (selectedAxis == NoneSelected)
        {
            return;
        }

        const glm::vec2 mouseLocation = ServiceLocator::GetInputService()->GetMousePosition();
        const glm::ivec2 mouseLocationInt = static_cast<glm::ivec2>(mouseLocation);
        const glm::ivec2 delta = clickLocation - mouseLocationInt;
        if (delta.x != 0 && delta.y != 0)
        {
            MouseMoved(delta.x, delta.y);
        }
    }

    void Gizmo::MouseMoved(const int x, const int y)
    {
        glm::vec3 axis;
        switch (selectedAxis)
        {
        case XSelected:
            axis = {1.0f, 0.0f, 0.0f};
            break;
        case YSelected:
            axis = {0.0f, 1.0f, 0.0f};
            break;
        case ZSelected:
            axis = {0.0f, 0.0f, 1.0f};
            break;
        case NoneSelected:
            break;
        }
        Ref<Camera> camera = ServiceLocator::GetRenderer()->GetCurrentCamera();
        glm::mat4x4 worldToScreen = camera->GetViewProjectionMatrix();
        glm::mat4x4 screenToWorld = glm::inverseTranspose(worldToScreen);

        const glm::vec2 mouseLocationPixels = ServiceLocator::GetInputService()->GetMousePosition();
        glm::vec2 mouseLocationViewport;
        ServiceLocator::GetEditorService()->GetEditor()->GetClickViewportSpace(
            mouseLocationViewport.x, mouseLocationViewport.y,
            static_cast<unsigned int>(mouseLocationPixels.x), static_cast<unsigned int>(mouseLocationPixels.y)
        );
        glm::vec4 gizmoLocationScreen =  worldToScreen * glm::vec4(transform.position + axis, 1.0f);
        gizmoLocationScreen /= gizmoLocationScreen.w;
        glm::vec4 mouseLocationWorld = glm::vec4(mouseLocationViewport, gizmoLocationScreen.z, 1.0f) * screenToWorld;
        mouseLocationWorld /= mouseLocationWorld.w;

        const float distance = glm::dot(axisScreenSpaceVector, glm::vec2(x, y)) * 0.01f;
        switch (selectedAxis)
        {
        case XSelected:
        {
            transform.position.x = mouseLocationWorld.x - 1.0f;
            break;
        }
        case YSelected:
        {
            transform.position.y = mouseLocationWorld.y - 1.0f;
            break;
        }
        case ZSelected:
        {
            transform.position.z = mouseLocationWorld.z - 1.0f;
            break;
        }
        default: break;
        }
        SetTransforms();
    }

    void Gizmo::SetTransforms()
    {
        Transform transformCopy = transform;
        yArrowMesh->SetTransform(transformCopy.GetMatrix());
        transformCopy.rotation = {glm::pi<float>() / 2.0f, glm::pi<float>() / 2.0f , 0.0f};
        xArrowMesh->SetTransform(transformCopy.GetMatrix());
        transformCopy.rotation = {glm::pi<float>() / 2.0f, 0.0f, 0.0f};
        zArrowMesh->SetTransform(transformCopy.GetMatrix());
    }

    void Gizmo::MouseReleased()
    {
        switch(selectedAxis)
        {
        case XSelected:
        {
            xArrowMesh->SetMaterial(0, red);
            break;
        }
        case YSelected:
        {
            yArrowMesh->SetMaterial(0, green);
            break;
        }
        case ZSelected:
        {
            zArrowMesh->SetMaterial(0, blue);
            break;
        }
        default: break;
        }
        selectedAxis = NoneSelected;

        originalTransform = transform;
    }

    void Gizmo::UpdateScreenSpaceVector(const glm::vec3 axis)
    {
        const glm::mat4x4& worldToView = ServiceLocator::GetRenderer()->GetCurrentCamera()->GetViewProjectionMatrix();
        glm::vec4 origin = worldToView * glm::vec4(transform.position, 1.0f);
        glm::vec4 offset = worldToView * glm::vec4(transform.position + axis, 1.0f);

        glm::vec2 origin2D = origin / origin.w;
        glm::vec2 offset2D = offset / offset.w;

        //axisScreenSpaceVector = glm::normalize(offset2D - origin2D);
        axisScreenSpaceVector = offset2D - origin2D;
        axisScreenSpaceVector.x *= -1.0f;
        VoxLog(Display, Input, "Gizmo clicked at '({:.2f}, {:.2f})", origin2D.x, origin2D.y);
        VoxLog(Display, Input, "Gizmo clicked, screen space vector '({:.2f}, {:.2f})", axisScreenSpaceVector.x, axisScreenSpaceVector.y);
    }
} // Vox