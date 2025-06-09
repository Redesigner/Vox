//
// Created by steph on 6/9/2025.
//

#include "AssetPtrDetailPanel.h"

#include <imgui.h>

#include "core/objects/properties/AssetPtr.h"
#include "core/services/ServiceLocator.h"
#include "rendering/Renderer.h"

namespace Vox
{
    bool AssetPtrDetailPanel::Draw(AssetPtr& assetPtr, const std::string& label)
    {
        bool ptrChanged = false;
        if (ImGui::BeginCombo(label.c_str(), assetPtr.path.string().c_str()))
        {
            switch (assetPtr.type)
            {
            case AssetPtr::AssetType::Mesh:
            {
                const auto& meshList = ServiceLocator::GetRenderer()->GetMeshes();
                for (const auto [name, mesh] : meshList)
                {
                    if (ImGui::Selectable(name.c_str()))
                    {
                        assetPtr.path = name;
                        ptrChanged = true;
                    }
                }
                break;
            }
            }
            ImGui::EndCombo();
        }

        return ptrChanged;
    }
} // Vox