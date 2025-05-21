#pragma once

#include <memory>
#include <string>

namespace Vox
{
    class Model;
    class MeshInstanceContainer;
	class SkeletalModel;
    class SkeletalMeshInstanceContainer;

	class AssetDisplayWindow
	{
	public:
		AssetDisplayWindow() = delete;

		static void Draw(bool* open);

	private:
		static void DrawMeshData(const std::pair<const std::string, std::shared_ptr<Model>>& mesh);

		static void DrawSkeletalMeshData(const std::pair<const std::string, std::shared_ptr<SkeletalModel>>& mesh);
	};
}
