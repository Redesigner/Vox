#pragma once

#include <functional>
#include <string>

#include <imgui.h>
#include <memory>

#include "editor/Console.h"

namespace Vox
{
    class ActorEditor;
    class EditorViewport;
}

struct ImFont;

namespace Vox
{
    class Gizmo;
    class Object;
	class World;
    class WorldOutline;
	class TestObject;
	class ColorDepthFramebuffer;

	class Editor
	{
	public:
		Editor();
	    ~Editor();

		void Draw();
		
		void BindOnGLTFOpened(std::function<void(std::string)> function);
		
		void SetWorld(const std::shared_ptr<World>& world);

		void SelectObject(const std::weak_ptr<Object>& object) const;

	    void InitializeGizmos(World* world);

	    [[nodiscard]] EditorViewport* GetViewport() const;

	    [[nodiscard]] std::weak_ptr<Object> GetSelectedObject() const;

		[[nodiscard]] static ImFont* GetFont_GitLab14();
		[[nodiscard]] static ImFont* GetFont_GitLab18();
		[[nodiscard]] static ImFont* GetFont_GitLab24();

	    static void BeginEmptyTab(const std::string& tabName);
	    static void EndEmptyTab();

	    static ImVec4 lightBgColor;
	    static ImVec4 mediumBgColor;
	    static ImVec4 darkBgColor;

	private:
		void DrawToolbar();
		void DrawFileToolbar();
		void DrawImportToolbar();

		void openGLTF();

		std::function<void(std::string)> onGLTFOpened = [](std::string string) {};

		static const char* gltfFilter[2];

		bool drawAssetViewer = false;

	    std::shared_ptr<EditorViewport> primaryViewport;
	    std::unique_ptr<ActorEditor> actorEditor;
		std::unique_ptr<Console> console;
	    std::unique_ptr<WorldOutline> worldOutline;
		std::weak_ptr<World> currentWorld;

		static ImFont* gitLabSans14;
		static ImFont* gitLabSans18;
		static ImFont* gitLabSans24;
	};
}
