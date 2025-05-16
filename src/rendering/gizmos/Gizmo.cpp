//
// Created by steph on 5/15/2025.
//

#include "Gizmo.h"

#include <glm/gtc/matrix_inverse.inl>

#include "core/logging/Logging.h"
#include "core/math/Math.h"
#include "core/services/EditorService.h"
#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"
#include "rendering/Camera.h"
#include "rendering/Renderer.h"

namespace Vox
{
    Gizmo::Gizmo()
        :red({1.0f, 0.1f, 0.0f, 1.0f}, 0.0f, 0.0f),
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

        mouseMotionCallback = ServiceLocator::GetInputService()->RegisterMouseMotionCallback([this](int x, int y)
        {
            MouseMoved();
        });

        xArrowMesh->RegisterClickCallback([this](const glm::ivec2 position)
        {
            xArrowMesh->SetMaterial(0, yellow);
            selectedAxis = XSelected;
        });

        yArrowMesh->RegisterClickCallback([this](const glm::ivec2 position)
        {
            yArrowMesh->SetMaterial(0, yellow);
            selectedAxis = YSelected;
        });

        zArrowMesh->RegisterClickCallback([this](const glm::ivec2 position)
        {
            zArrowMesh->SetMaterial(0, yellow);
            selectedAxis = ZSelected;
        });
    }

    Gizmo::~Gizmo()
    {
        ServiceLocator::GetInputService()->UnregisterMouseMotionCallback(mouseMotionCallback);
        ServiceLocator::GetInputService()->UnregisterMouseReleaseCallback(mouseReleaseCallback);
    }

    void Gizmo::SetTransform(const Transform& transformIn)
    {
        transform = transformIn;
        originalTransform = transformIn;
        SetTransforms();
    }

    void Gizmo::Update()
    {
        if (selectedAxis == NoneSelected)
        {
            return;
        }

        MouseMoved();
    }

    void Gizmo::SetVisible(const bool visible)
    {
        xArrowMesh->visible = visible;
        yArrowMesh->visible = visible;
        zArrowMesh->visible = visible;
    }

    Transform Gizmo::GetTransform() const
    {
        return transform;
    }

    void Gizmo::MouseMoved()
    {
        auto click = GetClickVector();

        switch (selectedAxis)
        {
        case XSelected:
        {
            const glm::vec3 intersection = ClosestPoint(
                originalTransform.position + glm::vec3(1.0f, 0.0f, 0.0f), {1.0f, 0.0f, 0.0f},
                click.first, click.second
            );
            transform.position.x = intersection.x - 1.0f;
            break;
        }
        case YSelected:
        {
            const glm::vec3 intersection = ClosestPoint(
                originalTransform.position + glm::vec3(0.0f, 1.0f, 0.0f), {0.0f, 1.0f, 0.0f},
                click.first, click.second
            );
            transform.position.y = intersection.y - 1.0f;
            break;
        }
        case ZSelected:
        {
            const glm::vec3 intersection = ClosestPoint(
                originalTransform.position + glm::vec3(0.0f, 0.0f, 1.0f), {0.0f, 0.0f, 1.0f},
                click.first, click.second
            );
            transform.position.z = intersection.z - 1.0f;
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

    std::pair<glm::vec3, glm::vec3> Gizmo::GetClickVector()
    {
        Ref<Camera> camera = ServiceLocator::GetRenderer()->GetCurrentCamera();
        glm::mat4x4 worldToScreen = camera->GetViewProjectionMatrix();
        glm::mat4x4 screenToWorld = glm::inverseTranspose(worldToScreen);

        const glm::vec2 mouseLocationPixels = ServiceLocator::GetInputService()->GetMousePosition();
        glm::vec2 mouseLocationViewport;
        ServiceLocator::GetEditorService()->GetEditor()->GetClickViewportSpace(
            mouseLocationViewport.x, mouseLocationViewport.y,
            static_cast<unsigned int>(mouseLocationPixels.x), static_cast<unsigned int>(mouseLocationPixels.y)
        );
        glm::vec4 mouseLocationWorldOrigin = glm::vec4(mouseLocationViewport, -1.0f, 1.0f) * screenToWorld;
        glm::vec4 mouseLocationWorldEnd = glm::vec4(mouseLocationViewport, 1.0f, 1.0f) * screenToWorld;
        mouseLocationWorldOrigin /= mouseLocationWorldOrigin.w; // affine
        mouseLocationWorldEnd /= mouseLocationWorldEnd.w;
        glm::vec3 delta = mouseLocationWorldEnd - mouseLocationWorldOrigin;
        return {mouseLocationWorldOrigin, glm::normalize(delta)};
    }
} // Vox