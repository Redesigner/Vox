#include "AssetDisplayWindow.h"

#include <ranges>
#include <imgui.h>

#include "core/services/ServiceLocator.h"
#include "rendering/mesh/MeshInstanceContainer.h"
#include "rendering/Renderer.h"

namespace Vox
{
	void AssetDisplayWindow::Draw(bool* open)
	{
		if (ImGui::Begin("Assets", open))
		{
			if (ImGui::CollapsingHeader("Static Meshes"))
			{
				for (const std::pair<const std::string, MeshInstanceContainer>& mesh : ServiceLocator::GetRenderer()->GetMeshes())
				{
					DrawMeshData(mesh);
				}
			}

			if (ImGui::CollapsingHeader("Skeletal Meshes"))
			{
				for (const auto& mesh : ServiceLocator::GetRenderer()->GetSkeletalMeshes())
				{
					DrawSkeletalMeshData(mesh);
				}
			}
		}
		ImGui::End();
	}

	void AssetDisplayWindow::DrawMeshData(const std::pair<const std::string, MeshInstanceContainer>& mesh)
	{
		if (ImGui::TreeNode(mesh.first.c_str()))
		{
			ImGui::Text("Instances: '%i'", mesh.second.GetInstanceCount());

			ImGui::TreePop();
		}
	}

    void AssetDisplayWindow::DrawSkeletalMeshData(
        const std::pair<const std::string, SkeletalMeshInstanceContainer>& mesh)
    {
		if (ImGui::TreeNode(mesh.first.c_str()))
		{
			if (ImGui::TreeNode("Animations"))
			{
				for (const auto& animation : mesh.second.GetAnimations())
				{
					ImGui::Text("%s : '%fs'", animation.GetName().c_str(), animation.GetDuration());
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}
}