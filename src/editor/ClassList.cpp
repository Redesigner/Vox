//
// Created by steph on 5/17/2025.
//

#include "ClassList.h"

#include <algorithm>
#include <imgui.h>

#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    void ClassList::Draw()
    {
        if (ImGui::BeginTabBar("ClassesTabs"))
        {
            if (ImGui::BeginTabItem("Classes"))
            {
                std::for_each(ServiceLocator::GetObjectService()->GetBegin(), ServiceLocator::GetObjectService()->GetEnd(),
                    [](const std::pair<std::string, ObjectClass>& entry)
                    {
                        ImGui::Selectable(entry.first.c_str());
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                        {
                            ImGui::SetDragDropPayload("OBJECT_CLASS_NAME", entry.first.data(), entry.first.size() + 1);
                            ImGui::EndDragDropSource();
                        }
                    });
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
} // Vox