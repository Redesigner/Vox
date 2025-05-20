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
        if (ImGui::Begin("Classes"))
        {
            std::for_each(ServiceLocator::GetObjectService()->GetBegin(), ServiceLocator::GetObjectService()->GetEnd(),
                [](const std::pair<std::string, ObjectClass>& entry)
                {
                    ImGui::Text(entry.first.c_str());
                });
        }
        ImGui::End();
    }
} // Vox