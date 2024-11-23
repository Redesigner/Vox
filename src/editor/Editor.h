#pragma once

#include "raylib.h"
#include "tinyfiledialogs.h"

#include <functional>
#include <string>

class Editor
{
public:
	void Draw(RenderTexture2D* viewportRenderTexture);
	void BindOnGLTFOpened(std::function<void(std::string)> function);
	Vector2 GetViewportDimensions() const;

private:
	void DrawToolbar();
	void DrawFileToolbar();
	void DrawImportToolbar();

	void openGLTF();

	std::function<void(std::string)> onGLTFOpened = [](std::string string) {};

	static const char* gltfFilter[2];

	Vector2 viewportDimensions = Vector2(0, 0);
};