//
// Created by steph on 5/17/2025.
//

#include "ClassList.h"

#include <algorithm>
#include <imgui.h>

#include "Editor.h"
#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    ClassList::ClassList()
    {
    }

    ClassList::~ClassList()
    {
    }

    void ClassList::Draw()
    {
        if (ImGui::BeginTabBar("ClassesTabs"))
        {
            if (ImGui::BeginTabItem("Classes"))
            {
                ImGui::PushStyleColor(ImGuiCol_ChildBg, Editor::lightBgColor);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 0.0f));
                ImGui::BeginChild("WorldOutlinePanel", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding);

                auto endItr = ServiceLocator::GetObjectService()->GetEnd();
                for (auto itr = ServiceLocator::GetObjectService()->GetBegin(); itr != endItr; ++itr)
                {
                    ImGui::Selectable(itr->first.c_str());
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                    {
                        ImGui::SetDragDropPayload("OBJECT_CLASS_NAME", itr->first.data(), itr->first.size() + 1);
                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::IsItemFocused() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        doubleClickCallback(&itr->second);
                    }
                }

                ImGui::PopStyleVar();
                ImGui::PopStyleColor();
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    void ClassList::SetDoubleClickCallback(ObjectCallback callback)
    {
        doubleClickCallback = std::move(callback);
    }
} // Vox