#include "Editor.h"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include "core/math/Math.h"
#include "core/objects/ObjectClass.h"
#include "core/objects/actor/Actor.h"
#include "detail_panel/DetailPanel.h"
#include "editor/AssetDisplayWindow.h"
#include "editor/WorldOutline.h"
#include "rendering/buffers/frame_buffers/ColorDepthFramebuffer.h"
#include "rendering/gizmos/Gizmo.h"

namespace Vox
{
    const char* Editor::gltfFilter[2] = { "*.gltf", "*.glb" };

    void Editor::Draw(const ColorDepthFramebuffer* viewportRenderTexture)
    {
        ImGui_ImplSDL3_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        {
#ifdef IMGUI_HAS_VIEWPORT
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->GetWorkPos());
            ImGui::SetNextWindowSize(viewport->GetWorkSize());
            ImGui::SetNextWindowViewport(viewport->ID);
#else 
             ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
             ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
#endif
            bool windowOpen = true;
            ImGui::PushFont(gitLabSans14);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, static_cast<ImVec4>(ImColor::HSV(0.0f, 0.0f, 0.2f)));
            ImGui::Begin("Main Window", &windowOpen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

            ImGui::SetScrollY(0);
            
            const ImVec2 dimensions = ImGui::GetContentRegionAvail();
            const ImVec2 bottomRight = ImGui::GetContentRegionMax();
            viewportDimensions = glm::vec2(dimensions.x, dimensions.y);
            viewportBox = Box(
                static_cast<int>(bottomRight.x - dimensions.x),
                static_cast<int>(bottomRight.y - dimensions.y),
                static_cast<int>(bottomRight.x),
                static_cast<int>(bottomRight.y));
            ImGui::Image(viewportRenderTexture->GetTextureId(),
                ImVec2(static_cast<float>(viewportRenderTexture->GetWidth()), static_cast<float>(viewportRenderTexture->GetHeight())),
                {0, 1}, {1, 0});
            if (ImGui::IsItemHovered())
            {
                ImGui::SetNextFrameWantCaptureMouse(false);
            }
            
            DrawToolbar();
            if (console)
            {
                console->Draw();
            }

            if (drawAssetViewer)
            {
                AssetDisplayWindow::Draw(&drawAssetViewer);
            }

            ImGui::End();
            ImGui::PopStyleColor();
            ImGui::PopStyleVar(2);

            if (worldOutline->GetSelectedObject())
            {
                DetailPanel::Draw(worldOutline->GetSelectedObject());
            }

            if (!currentWorld.expired())
            {
                worldOutline->Draw(currentWorld.lock().get());
            }
            
            ImGui::PopFont();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Editor::BindOnGLTFOpened(std::function<void(std::string)> function)
    {
        onGLTFOpened = function;
    }

    glm::vec2 Editor::GetViewportDimensions() const
    {
        return viewportDimensions;
    }

    Editor::Box Editor::GetViewportBox() const
    {
        return viewportBox;
    }

    bool Editor::GetClickViewportSpace(float& xOut, float& yOut, unsigned int clickX, unsigned int clickY) const
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

    bool Editor::GetClickViewportSpace(unsigned int& xOut, unsigned int& yOut, unsigned int clickX, unsigned int clickY) const
    {
        if (clickY < viewportBox.top || clickY > viewportBox.bottom ||
            clickX < viewportBox.left || clickX > viewportBox.right)
        {
            return false;
        }

        xOut = clickX - viewportBox.left;
        yOut = clickY - viewportBox.top;
        VoxLog(Display, Input, "Mouse clicked at '({}, {})'px, remapped to '({}, {})px", clickX, clickY, xOut, yOut);
        return true;
    }

    void Editor::SetWorld(const std::shared_ptr<World>& world)
    {
        currentWorld = world;
    }

    ImFont* Editor::GetFont_GitLab18()
    {
        return gitLabSans18;
    }

    ImFont* Editor::GetFont_GitLab24()
    {
        return gitLabSans24;
    }

    void Editor::DrawToolbar()
    {
        if (ImGui::BeginMenuBar())
        {
            DrawFileToolbar();
            ImGui::MenuItem("Edit");
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("Asset List Viewer"))
                {
                    drawAssetViewer = !drawAssetViewer;
                }
                ImGui::EndMenu();
            }
            ImGui::PopStyleVar();
            DrawImportToolbar();
            ImGui::EndMenuBar();
        }
    }

    void Editor::DrawFileToolbar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("New");
            ImGui::MenuItem("Open");
            ImGui::MenuItem("Save");
            ImGui::Separator();
            ImGui::MenuItem("Exit");
            ImGui::EndMenu();
        }
        ImGui::PopStyleVar();
    }

    void Editor::DrawImportToolbar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
        if (ImGui::BeginMenu("Import"))
        {
            if (ImGui::MenuItem("Model (gltf)"))
            {
                openGLTF();
            }
            ImGui::EndMenu();
        }
        ImGui::PopStyleVar();
    }
    

    void Editor::openGLTF()
    {

        //char* fileNameRaw = tinyfd_openFileDialog(NULL, NULL, 2, gltfFilter, "GLTF (*.gltf, *.glb)", false);
        //if (fileNameRaw)
        //{
        //    onGLTFOpened(fileNameRaw);
        //}
        //else
        //{
        //    VoxLog(Display, Input, "Dialog closed without selecting file.");
        //}
    }

    void Editor::SelectObject(Object* object) const
    {
        worldOutline->SetSelectedObject(object);
    }

    void Editor::InitializeGizmos()
    {
        worldOutline->InitializeGizmos();
    }

    Object* Editor::GetSelectedObject() const
    {
        return worldOutline->GetSelectedObject();
    }

    Editor::Box::Box()
        :left(0), top(0), right(0), bottom(0)
    {
    }

    Editor::Box::Box(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)
        :left(left), top(top), right(right), bottom(bottom)
    {
    }

    Editor::Editor()
    {
        console = std::make_unique<Console>();
        worldOutline = std::make_unique<WorldOutline>();

        const ImGuiIO& io = ImGui::GetIO();
        gitLabSans14 = io.Fonts->AddFontFromFileTTF("../../../assets/fonts/GitLabSans.ttf", 14);
        gitLabSans18 = io.Fonts->AddFontFromFileTTF("../../../assets/fonts/GitLabSans.ttf", 18);
        gitLabSans24 = io.Fonts->AddFontFromFileTTF("../../../assets/fonts/GitLabSans.ttf", 24);
    }

    Editor::~Editor()
    {
    }

    ImFont* Editor::gitLabSans14 = nullptr;
    ImFont* Editor::gitLabSans18 = nullptr;
    ImFont* Editor::gitLabSans24 = nullptr;
}
