#pragma once

#include <functional>
#include <string>

#include <glm/vec2.hpp>
#include <memory>

#include "editor/Console.h"

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
		struct Box
		{
			Box();
			Box(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom);

			unsigned int left, top, right, bottom;
		};

		Editor();
	    ~Editor();

		void Draw(const ColorDepthFramebuffer* viewportRenderTexture);
		
		void BindOnGLTFOpened(std::function<void(std::string)> function);
		
		glm::vec2 GetViewportDimensions() const;
		
		Box GetViewportBox() const;
		
		bool GetClickViewportSpace(float& xOut, float& yOut, unsigned int clickX, unsigned int clickY) const;

		bool GetClickViewportSpace(unsigned int& xOut, unsigned int& yOut, unsigned int clickX, unsigned int clickY) const;
		
		void SetWorld(const std::shared_ptr<World>& world);

		void SelectObject(Object* object);

	    void InitializeGizmos();

	    Object* GetSelectedObject() const;

		static ImFont* GetFont_GitLab18();
		static ImFont* GetFont_GitLab24();

	private:
		void DrawToolbar();
		void DrawFileToolbar();
		void DrawImportToolbar();

		void openGLTF();

		std::function<void(std::string)> onGLTFOpened = [](std::string string) {};

		static const char* gltfFilter[2];

		bool drawAssetViewer = false;

		std::unique_ptr<Console> console;
	    std::unique_ptr<WorldOutline> worldOutline;
	    std::unique_ptr<Gizmo> gizmo;
		
		glm::vec2 viewportDimensions = glm::vec2(800.0f, 450.0f);
		Box viewportBox{ 0, 0, 800, 450 };

		std::weak_ptr<World> currentWorld;
		
		static ImFont* gitLabSans14;
		static ImFont* gitLabSans18;
		static ImFont* gitLabSans24;
	};
}
