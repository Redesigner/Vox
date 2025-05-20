#include "Editor.h"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include "ClassList.h"
#include "EditorViewport.h"
#include "core/math/Math.h"
#include "core/objects/ObjectClass.h"
#include "detail_panel/DetailPanel.h"
#include "editor/AssetDisplayWindow.h"
#include "editor/WorldOutline.h"
#include "rendering/gizmos/Gizmo.h"

namespace Vox
{
    const char* Editor::gltfFilter[2] = { "*.gltf", "*.glb" };

    Editor::Editor()
    {
        console = std::make_unique<Console>();
        worldOutline = std::make_unique<WorldOutline>();
        viewport = std::make_unique<EditorViewport>();

        const ImGuiIO& io = ImGui::GetIO();
        gitLabSans14 = io.Fonts->AddFontFromFileTTF("../../../assets/fonts/GitLabSans.ttf", 14);
        gitLabSans18 = io.Fonts->AddFontFromFileTTF("../../../assets/fonts/GitLabSans.ttf", 18);
        gitLabSans24 = io.Fonts->AddFontFromFileTTF("../../../assets/fonts/GitLabSans.ttf", 24);
    }

    Editor::~Editor()
    = default;

    void Editor::Draw(const ColorDepthFramebuffer* viewportRenderTexture)
    {
        ImGui_ImplSDL3_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::PushFont(gitLabSans14);

            viewport->Draw(viewportRenderTexture);

            //DrawToolbar();

            if (console)
            {
                console->Draw();
            }

            if (drawAssetViewer)
            {
                AssetDisplayWindow::Draw(&drawAssetViewer);
            }

            if (!worldOutline->GetSelectedObject().expired())
            {
                DetailPanel::Draw(worldOutline->GetSelectedObject().lock().get());
            }

            if (!currentWorld.expired())
            {
                worldOutline->Draw(currentWorld.lock().get());
            }

            ClassList::Draw();

            ImGui::PopFont();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Editor::BindOnGLTFOpened(std::function<void(std::string)> function)
    {
        onGLTFOpened = function;
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

    void Editor::SelectObject(const std::weak_ptr<Object>& object) const
    {
        worldOutline->SetSelectedObject(object);
    }

    void Editor::InitializeGizmos()
    {
        worldOutline->InitializeGizmos();
    }

    EditorViewport* Editor::GetViewport() const
    {
        return viewport.get();
    }

    std::weak_ptr<Object> Editor::GetSelectedObject() const
    {
        return worldOutline->GetSelectedObject();
    }


    ImFont* Editor::gitLabSans14 = nullptr;
    ImFont* Editor::gitLabSans18 = nullptr;
    ImFont* Editor::gitLabSans24 = nullptr;
}
