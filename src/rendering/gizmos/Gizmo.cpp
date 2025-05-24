//
// Created by steph on 5/15/2025.
//

#include "Gizmo.h"

#include <glm/gtc/matrix_inverse.inl>

#include "core/math/Math.h"
#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"
#include "editor/EditorViewport.h"
#include "../camera/Camera.h"
#include "rendering/Renderer.h"
#include "rendering/SceneRenderer.h"

namespace Vox
{
    Gizmo::Gizmo(SceneRenderer* scene)
        :red({1.0f, 0.25f, 0.25f, 1.0f}, 0.0f, 0.0f),
        green({0.25f, 1.0f, 0.25f, 1.0f}, 0.0f, 0.0f),
        blue({0.25f, 0.25f, 1.0f, 1.0f}, 0.0f, 0.0f),
        selected({0.8f, 0.8f, 0.95f, 1.0f}, 0.0f, 0.0f)
    {
        xArrowMesh = scene->CreateMeshInstance("gizmoArrow");
        yArrowMesh = scene->CreateMeshInstance("gizmoArrow");
        zArrowMesh = scene->CreateMeshInstance("gizmoArrow");

        xArrowMesh->SetMaterial(0, red);
        yArrowMesh->SetMaterial(0, green);
        zArrowMesh->SetMaterial(0, blue);

        xArrowMesh->visible = false;
        yArrowMesh->visible = false;
        zArrowMesh->visible = false;

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
            xArrowMesh->SetMaterial(0, selected);
            selectedAxis = XSelected;
        });

        yArrowMesh->RegisterClickCallback([this](const glm::ivec2 position)
        {
            yArrowMesh->SetMaterial(0, selected);
            selectedAxis = YSelected;
        });

        zArrowMesh->RegisterClickCallback([this](const glm::ivec2 position)
        {
            zArrowMesh->SetMaterial(0, selected);
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
        SceneRenderer* scene = xArrowMesh->GetMeshOwner()->GetOwner();

        if (visible)
        {
            scene->RegisterOverlayMesh(xArrowMesh);
            scene->RegisterOverlayMesh(yArrowMesh);
            scene->RegisterOverlayMesh(zArrowMesh);
        }
        else
        {
            scene->ClearOverlays();
        }
    }

    Transform Gizmo::GetTransform() const
    {
        return transform;
    }

    void Gizmo::MouseMoved()
    {
        glm::vec3 position, direction;
        if (!GetClickVector(position, direction))
        {
            return;
        }

        switch (selectedAxis)
        {
        case XSelected:
        {
            const glm::vec3 intersection = ClosestPoint(
                originalTransform.position + glm::vec3(1.0f, 0.0f, 0.0f), {1.0f, 0.0f, 0.0f},
                position, direction
            );
            transform.position.x = intersection.x - 1.0f;
            break;
        }
        case YSelected:
        {
            const glm::vec3 intersection = ClosestPoint(
                originalTransform.position + glm::vec3(0.0f, 1.0f, 0.0f), {0.0f, 1.0f, 0.0f},
                position, direction
            );
            transform.position.y = intersection.y - 1.0f;
            break;
        }
        case ZSelected:
        {
            const glm::vec3 intersection = ClosestPoint(
                originalTransform.position + glm::vec3(0.0f, 0.0f, 1.0f), {0.0f, 0.0f, 1.0f},
                position, direction
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

    bool Gizmo::GetClickVector(glm::vec3& positionOut, glm::vec3& directionOut)
    {
        std::shared_ptr<Camera> camera = xArrowMesh->GetMeshOwner()->GetOwner()->GetCurrentCamera();
        glm::mat4x4 worldToScreen = camera->GetViewProjectionMatrix();
        glm::mat4x4 screenToWorld = glm::inverseTranspose(worldToScreen);

        // This won't work right now!
        const glm::vec2 mouseLocationPixels = ServiceLocator::GetInputService()->GetMousePosition();
        glm::vec2 mouseLocationViewport;
        if (!xArrowMesh->GetMeshOwner()->GetOwner()->viewport.lock()->GetClickViewportSpace(
            mouseLocationViewport.x, mouseLocationViewport.y,
            static_cast<unsigned int>(mouseLocationPixels.x), static_cast<unsigned int>(mouseLocationPixels.y)
        ))
        {
            return false;
        }
        glm::vec4 mouseLocationWorldOrigin = glm::vec4(mouseLocationViewport, -1.0f, 1.0f) * screenToWorld;
        glm::vec4 mouseLocationWorldEnd = glm::vec4(mouseLocationViewport, 1.0f, 1.0f) * screenToWorld;
        mouseLocationWorldOrigin /= mouseLocationWorldOrigin.w; // affine
        mouseLocationWorldEnd /= mouseLocationWorldEnd.w;
        glm::vec3 delta = mouseLocationWorldEnd - mouseLocationWorldOrigin;
        positionOut = mouseLocationWorldOrigin;
        directionOut = glm::normalize(delta);
        return true;
    }
} // Vox