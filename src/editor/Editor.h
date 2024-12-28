#pragma once

#include "raylib.h"
#include "tinyfiledialogs.h"

#include <functional>
#include <string>

class Editor
{
public:
	struct Box
	{
		Box();
		Box(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom);

		unsigned int left, top, right, bottom;
	};
	
	void Draw(RenderTexture2D* viewportRenderTexture);
	void BindOnGLTFOpened(std::function<void(std::string)> function);
	Vector2 GetViewportDimensions() const;
	Box GetViewportBox() const;
	bool GetClickViewportSpace(float& xOut, float& yOut, unsigned int clickX, unsigned int clickY) const;

private:
	void DrawToolbar();
	void DrawFileToolbar();
	void DrawImportToolbar();

	void openGLTF();

	std::function<void(std::string)> onGLTFOpened = [](std::string string) {};

	static const char* gltfFilter[2];

	Vector2 viewportDimensions = Vector2(0.0f, 0.0f);
	Box viewportBox;
};