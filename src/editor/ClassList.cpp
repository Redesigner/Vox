//
// Created by steph on 5/17/2025.
//

#include "ClassList.h"

#include <algorithm>
#include <utility>
#include <imgui.h>

#include "Editor.h"
#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    ClassList::ClassList()
    {
        selectedObjectClass = nullptr;
    }

    ClassList::~ClassList()
    = default;

    void ClassList::Draw()
    {
        if (ImGui::BeginTabBar("ClassesTabs"))
        {
            if (ImGui::BeginTabItem(title.c_str()))
            {
                ImGui::PushStyleColor(ImGuiCol_ChildBg, Editor::lightBgColor);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 0.0f));
                ImGui::BeginChild("WorldOutlinePanel", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding);

                auto endItr = ServiceLocator::GetObjectService()->GetEnd();
                for (auto itr = ServiceLocator::GetObjectService()->GetBegin(); itr != endItr; ++itr)
                {
                    if (!classFilter(itr->second))
                    {
                        continue;;
                    }

                    ImGui::Selectable(itr->first.c_str());
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                    {
                        ImGui::SetDragDropPayload(objectClassPayloadType.c_str(), itr->first.data(), itr->first.size() + 1);
                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::IsItemFocused() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        doubleClickCallback(itr->second);
                    }

                    if (ImGui::BeginPopupContextItem())
                    {
                        ImGui::Selectable("New prefab from this");
                        if (itr->second->CanBeRenamed())
                        {
                            ImGui::Text("Edit name");
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::EndChild();
                ImGui::PopStyleVar();
                ImGui::PopStyleColor();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    void ClassList::SetDoubleClickCallback(ObjectCallback callback)
    {
        doubleClickCallback = std::move(callback);
    }

    void ClassList::SetClassFilter(std::function<bool(const std::shared_ptr<ObjectClass>&)> filter)
    {
        classFilter = std::move(filter);
    }
} // Vox