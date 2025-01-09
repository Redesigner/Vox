#include "Editor.h"

#include "imgui.h"
#include "tinyfiledialogs.h"

#include "core/logging/Logging.h"
#include "core/math/Math.h"

const char* Editor::gltfFilter[2] = { "*.gltf", "*.glb" };

void Editor::Draw(RenderTexture2D* viewportRenderTexture)
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
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, static_cast<ImVec4>(ImColor::HSV(0.0f, 0.0f, 0.2f)));
    ImGui::Begin("Main Window", &windowOpen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar);

    ImVec2 dimensions = ImGui::GetContentRegionAvail();
    ImVec2 bottomRight = ImGui::GetContentRegionMax();
    viewportDimensions = Vector2(dimensions.x , dimensions.y);
    viewportBox = Box(bottomRight.x - dimensions.x, bottomRight.y - dimensions.y, bottomRight.x, bottomRight.y);

    rlImGuiImageRenderTextureFit(viewportRenderTexture, false);

	DrawToolbar();
    DrawDebugConsole();

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

Editor::Box Editor::GetViewportBox() const
{
    return viewportBox;
}

bool Editor::GetClickViewportSpace(float& xOut, float& yOut, unsigned int clickX, unsigned int clickY) const
{
    if ((clickY < viewportBox.top || clickY > viewportBox.bottom) ||
        (clickX < viewportBox.left || clickX > viewportBox.right))
    {
        return false;
    }

    xOut = Vox::RemapRange(clickX, viewportBox.left, viewportBox.right, -1.0f, 1.0f);
    yOut = Vox::RemapRange(clickY, viewportBox.top, viewportBox.bottom, 1.0f, -1.0f);
    return true;
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

void Editor::DrawDebugConsole()
{
    const float consoleHeight = ImGui::GetTextLineHeightWithSpacing() * 8;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.7f));
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - consoleHeight - 5.0f);
    if (ImGui::BeginChild("ResizableChild", ImVec2(-FLT_MIN, consoleHeight), ImGuiChildFlags_Borders))
    {
        for (const Vox::LogEntry& entry : Vox::Logger::GetEntries())
        {
            glm::vec3 entryColor = Vox::Logger::GetLevelColor(entry.level);
            ImGui::TextColored(ImVec4(entryColor.r / 255.0f, entryColor.g / 255.0f, entryColor.b / 255.0f, 1.0f), entry.entry.c_str());
        }
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();
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
        VoxLog(Display, Input, "Dialog closed without selecting file.");
    }
}

Editor::Box::Box()
    :left(0), top(0), right(0), bottom(0)
{
}

Editor::Box::Box(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)
    :left(left), top(top), right(right), bottom(bottom)
{
}
