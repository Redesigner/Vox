#include "Editor.h"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include "../../out/build/default/vcpkg_installed/vcpkg/blds/imgui/src/v1.91.9-afb09617a6.clean/imgui_internal.h"
#include "ClassList.h"
#include "EditorViewport.h"
#include "actor_editor/ActorEditor.h"
#include "detail_panel/DetailPanel.h"
#include "editor/AssetDisplayWindow.h"
#include "editor/WorldOutline.h"
#include "rendering/SceneRenderer.h"
#include "rendering/gizmos/Gizmo.h"

namespace Vox
{
    const char* Editor::gltfFilter[2] = { "*.gltf", "*.glb" };

    Editor::Editor()
    {
        console = std::make_unique<Console>();
        worldOutline = std::make_unique<WorldOutline>();
        primaryViewport = std::make_shared<EditorViewport>();
        primaryViewport->OnObjectSelected = [this](const std::shared_ptr<Object>& object)
            {
                worldOutline->SetSelectedObject(object);
            };
        actorEditor = std::make_unique<ActorEditor>();
        classList = std::make_unique<ClassList>();
        classList->SetDoubleClickCallback([this](const ObjectClass& objectClass)
            {
               pendingEditorClass = &objectClass;
            });

        const ImGuiIO& io = ImGui::GetIO();
        gitLabSans14 = io.Fonts->AddFontFromFileTTF("../../../assets/fonts/GitLabSans.ttf", 14);
        gitLabSans18 = io.Fonts->AddFontFromFileTTF("../../../assets/fonts/GitLabSans.ttf", 18);
        gitLabSans24 = io.Fonts->AddFontFromFileTTF("../../../assets/fonts/GitLabSans.ttf", 24);
    }

    Editor::~Editor()
    = default;

    ImVec4 Editor::lightBgColor = ImVec4(ImColor(43, 45, 48));
    ImVec4 Editor::mediumBgColor = ImVec4(ImColor(38, 38, 38));
    ImVec4 Editor::darkBgColor = ImVec4(ImColor(30, 31, 34));

    void Editor::Draw()
    {
        bool actorEditorOpen = true;

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

            ImGui::PushStyleColor(ImGuiCol_ChildBg, mediumBgColor);
            ImGui::PushStyleColor(ImGuiCol_Border, darkBgColor);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, darkBgColor);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::PushFont(gitLabSans14);

            if (ImGui::Begin("Main", nullptr,
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize /*| ImGuiWindowFlags_MenuBar*/ | ImGuiWindowFlags_NoBringToFrontOnFocus))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 4.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                ImGui::BeginChild("MainUpper", ImVec2(0, 400), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY);
                {
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
                    constexpr ImGuiChildFlags flags = ImGuiChildFlags_AlwaysUseWindowPadding;
                    ImGui::BeginChild("MainClassList", ImVec2(100, 0), flags | ImGuiChildFlags_ResizeX);
                    classList->Draw();
                    ImGui::EndChild();

                    ImGui::SameLine();
                    ImGui::BeginChild("MainViewport", ImVec2(400, 0), flags | ImGuiChildFlags_ResizeX);
                    primaryViewport->Draw(currentWorld.lock());
                    ImGui::EndChild();

                    ImGui::SameLine();
                    ImGui::BeginChild("MainWorldOutlineDetails", ImVec2(0, 0), flags);
                    {
                        ImGui::BeginChild("MainWorldOutline", ImVec2(0, 300), ImGuiChildFlags_ResizeY);
                        if (!currentWorld.expired())
                        {
                            worldOutline->Draw(currentWorld.lock().get());
                        }
                        ImGui::EndChild();

                        ImGui::BeginChild("MainDetailsPanel");
                        const std::string tabName = fmt::format("{}###DetailPanelTab", worldOutline->GetSelectedObject().expired() ?
                            "No Object Selected" :
                            worldOutline->GetSelectedObject().lock()->GetDisplayName());
                        BeginEmptyTab(tabName);
                        if (!worldOutline->GetSelectedObject().expired())
                        {
                            DetailPanel::Draw(worldOutline->GetSelectedObject().lock().get());
                        }
                        EndEmptyTab();

                        ImGui::EndChild();
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleVar(2);
                }
                ImGui::EndChild();
                ImGui::BeginChild("MainBottomTab", ImVec2(0, 0));
                if (console)
                {
                    console->Draw();
                }
                ImGui::EndChild();
                ImGui::PopStyleVar(2);
            }
            ImGui::End();

            if (drawAssetViewer)
            {
                AssetDisplayWindow::Draw(&drawAssetViewer);
            }

            if (actorEditor)
            {
                if (ImGui::Begin("Actor Editor", &actorEditorOpen))
                {
                    actorEditor->Draw();
                }
                ImGui::End();
            }
            //DrawToolbar();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
        ImGui::PopFont();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (!actorEditorOpen)
        {
            actorEditor.reset();
        }

        if (pendingEditorClass)
        {
            actorEditor = std::make_unique<ActorEditor>();
            pendingEditorClass = nullptr;
        }
    }

    void Editor::BindOnGLTFOpened(std::function<void(std::string)> function)
    {
        onGLTFOpened = function;
    }

    void Editor::SetWorld(const std::shared_ptr<World>& world)
    {
        currentWorld = world;
        world->GetRenderer()->viewport = primaryViewport;
    }

    ImFont* Editor::GetFont_GitLab14()
    {
        return gitLabSans14;
    }

    ImFont* Editor::GetFont_GitLab18()
    {
        return gitLabSans18;
    }

    ImFont* Editor::GetFont_GitLab24()
    {
        return gitLabSans24;
    }

    void Editor::BeginEmptyTab(const std::string& tabName)
    {
        ImGui::PushFont(GetFont_GitLab18());
        ImGui::BeginTabBar((tabName + "Bar").c_str());
        ImGui::BeginTabItem(tabName.c_str());
        ImGui::PushFont(GetFont_GitLab14());
    }

    void Editor::EndEmptyTab()
    {
        ImGui::PopFont();
        ImGui::EndTabItem();
        ImGui::EndTabBar();
        ImGui::PopFont();
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

    void Editor::InitializeGizmos(World* world)
    {
        worldOutline->InitializeGizmos(world);
    }

    EditorViewport* Editor::GetViewport() const
    {
        return primaryViewport.get();
    }

    std::weak_ptr<Object> Editor::GetSelectedObject() const
    {
        return worldOutline->GetSelectedObject();
    }


    ImFont* Editor::gitLabSans14 = nullptr;
    ImFont* Editor::gitLabSans18 = nullptr;
    ImFont* Editor::gitLabSans24 = nullptr;
}
