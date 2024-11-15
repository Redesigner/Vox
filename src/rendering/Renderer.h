#pragma once

#include "raylib.h"

#include <memory>
#include <string>

#include "rendering/Light.h"

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

	void RenderGBuffer();

	void RenderDeferred();

	std::unique_ptr<GBuffer> gBuffer;

	Shader gBufferShader;
	Shader deferredShader;

	LightUniformLocations lightUniformLocations;
	Light testLight;

	Camera3D camera;
	RenderTexture2D viewportTexture;
	Model testModel;
};