#pragma once

#include <string>

namespace Vox
{
	class MeshInstanceContainer;
	class SkeletalModel;

	class AssetDisplayWindow
	{
	public:
		AssetDisplayWindow() = delete;

		static void Draw();

	private:
		static void DrawMeshData(const std::pair<const std::string, MeshInstanceContainer>& mesh);

		static void DrawSkeletalMeshData(const std::pair<const std::string, SkeletalModel>& mesh);
	};
}