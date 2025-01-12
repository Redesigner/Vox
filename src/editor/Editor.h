#pragma once

#include <functional>
#include <string>

#include <glm/vec2.hpp>
#include "tinyfiledialogs.h"

namespace Vox
{
	class RenderTexture;

	class Editor
	{
	public:
		struct Box
		{
			Box();
			Box(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom);

			unsigned int left, top, right, bottom;
		};

		void Draw(RenderTexture* viewportRenderTexture);
		void BindOnGLTFOpened(std::function<void(std::string)> function);
		glm::vec2 GetViewportDimensions() const;
		Box GetViewportBox() const;
		bool GetClickViewportSpace(float& xOut, float& yOut, unsigned int clickX, unsigned int clickY) const;

	private:
		void DrawToolbar();
		void DrawFileToolbar();
		void DrawImportToolbar();
		void DrawDebugConsole();

		void openGLTF();

		std::function<void(std::string)> onGLTFOpened = [](std::string string) {};

		static const char* gltfFilter[2];

		glm::vec2 viewportDimensions = glm::vec2(0.0f, 0.0f);
		Box viewportBox;
	};
}