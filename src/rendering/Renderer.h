#pragma once

#include "raylib.h"

#include <memory>
#include <string>

class Editor;
class GBuffer;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Render(Editor* editor);

	void LoadTestModel(std::string path);

private:
	void UpdateViewportDimensions(Editor* editor);

	std::unique_ptr<GBuffer> gBuffer;

	Camera3D camera;
	RenderTexture2D viewportTexture;
	Model testModel;
};