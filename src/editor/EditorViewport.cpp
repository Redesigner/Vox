//
// Created by steph on 5/19/2025.
//

#include "EditorViewport.h"

#include <imgui.h>

#include "core/math/Math.h"
#include "core/services/EditorService.h"
#include "rendering/SceneRenderer.h"
#include "rendering/buffers/frame_buffers/ColorDepthFramebuffer.h"

namespace Vox
{
    ViewportBox::ViewportBox()
        :left(0), top(0), right(0), bottom(0)
    {
    }

    ViewportBox::ViewportBox(const unsigned int left, const unsigned int top, const unsigned int right, const unsigned int bottom)
        :left(left), top(top), right(right), bottom(bottom)
    {
    }

    EditorViewport::EditorViewport()
    = default;

    void EditorViewport::Draw(const std::shared_ptr<World>& world)
    {
#ifdef IMGUI_HAS_VIEWPORT
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetWorkPos());
        ImGui::SetNextWindowSize(viewport->GetWorkSize());
        ImGui::SetNextWindowViewport(viewport->ID);
#else
        // ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        // ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
#endif
        bool windowOpen = true;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, static_cast<ImVec4>(ImColor::HSV(0.0f, 0.0f, 0.2f)));

        if (ImGui::Begin("Main Window", &windowOpen/*,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus*/))
        {
            ImGui::SetScrollY(0);
            const ImVec2 dimensions = ImGui::GetContentRegionAvail();
            const ImVec2 bottomRight = ImGui::GetContentRegionMax();
            viewportDimensions = glm::vec2(dimensions.x, dimensions.y);
            viewportBox = ViewportBox
            (
                static_cast<int>(bottomRight.x - dimensions.x),
                static_cast<int>(bottomRight.y - dimensions.y),
                static_cast<int>(bottomRight.x),
                static_cast<int>(bottomRight.y)
            );

            ColorDepthFramebuffer* texture = world->GetRenderer()->GetTexture();
            ImGui::Image(texture->GetTextureId(),
                ImVec2(static_cast<float>(texture->GetWidth()), static_cast<float>(texture->GetHeight())),
                {0, 1}, {1, 0});
            if (ImGui::IsItemHovered())
            {
                ImGui::SetNextFrameWantCaptureMouse(false);
            }

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OBJECT_CLASS_NAME"))
                {
                    const char* objectClassName = static_cast<const char*>(payload->Data);
                    world->CreateObject(std::string(objectClassName));
                }
                ImGui::EndDragDropTarget();
            }

            const ImVec2 min = ImGui::GetWindowContentRegionMin();
            const ImVec2 max = ImGui::GetWindowContentRegionMax();
            world->GetRenderer()->SetSize(max.x - min.x, max.y - min.y);
        }
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }

    bool EditorViewport::GetClickViewportSpace(float& xOut, float& yOut, unsigned int clickX, unsigned int clickY) const
    {
        if (clickY < viewportBox.top || clickY > viewportBox.bottom ||
            clickX < viewportBox.left || clickX > viewportBox.right)
        {
            return false;
        }

        xOut = RemapRange(clickX, viewportBox.left, viewportBox.right, -1.0f, 1.0f);
        yOut = RemapRange(clickY, viewportBox.top, viewportBox.bottom, 1.0f, -1.0f);
        return true;
    }

    bool EditorViewport::GetClickViewportSpace(unsigned int& xOut, unsigned int& yOut, unsigned int clickX, unsigned int clickY) const
    {
        if (clickY < viewportBox.top || clickY > viewportBox.bottom ||
            clickX < viewportBox.left || clickX > viewportBox.right)
        {
            return false;
        }

        xOut = clickX - viewportBox.left;
        yOut = clickY - viewportBox.top;
        return true;
    }

    glm::vec2 EditorViewport::GetDimensions() const
    {
        return viewportDimensions;
    }

    ViewportBox EditorViewport::GetViewportBox() const
    {
        return viewportBox;
    }
} // Vox