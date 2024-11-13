#include "Editor.h"

#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "tinyfiledialogs.h"

const char* Editor::gltfFilter[2] = { "*.gltf", "*.glb" };

void Editor::Draw(RenderTexture2D* viewportRenderTexture)
{
    // start ImGui Conent
    rlImGuiBegin();

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
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, static_cast<ImVec4>(ImColor::HSV(0.0f, 0.0f, 0.2f)));
    ImGui::Begin("Main Window", &windowOpen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar);

    ImVec2 dimensions = ImGui::GetContentRegionAvail();
    viewportDimensions = Vector2(dimensions.x, dimensions.y);

    rlImGuiImageRenderTextureFit(viewportRenderTexture, true);

	DrawToolbar();

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);

    rlImGuiEnd();
}

void Editor::BindOnGLTFOpened(std::function<void(std::string)> function)
{
    onGLTFOpened = function;
}

Vector2 Editor::GetViewportDimensions() const
{
    return viewportDimensions;
}

void Editor::DrawToolbar()
{
    if (ImGui::BeginMenuBar())
    {
        DrawFileToolbar();
        ImGui::MenuItem("Edit");
        ImGui::MenuItem("Tools");
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

    char* fileNameRaw = tinyfd_openFileDialog(NULL, NULL, 2, gltfFilter, "GLTF (*.gltf, *.glb)", false);
    if (fileNameRaw)
    {
        onGLTFOpened(fileNameRaw);
    }
    else
    {
        TraceLog(LOG_INFO, "Dialog closed without selecting file.");
    }
}
