#pragma once

#include "raylib.h"

#include <memory>
#include <string>

#include "rendering/Light.h"

class DeferredShader;
class Editor;
class Framebuffer;
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

	void RenderSky();

	void CopyViewportToTexture(RenderTexture2D& texture);

	std::unique_ptr<GBuffer> gBuffer;
	std::unique_ptr<Framebuffer> deferredFramebuffer;

	Shader gBufferShader;
	std::unique_ptr<DeferredShader> deferredShader;
	Shader skyShader;

	LightUniformLocations lightUniformLocations;
	Light testLight;
	int materialColorLocation, materialRoughnessLocation = 0;

	Camera3D camera;
	RenderTexture2D viewportTexture;
	Material defaultMaterial;
	Model testModel;
	Mesh voxelMesh;
};