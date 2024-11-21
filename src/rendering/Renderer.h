#pragma once

#include "raylib.h"

#include <memory>
#include <string>

#include "rendering/Light.h"

class ArrayTexture;
class DeferredShader;
class Editor;
class Framebuffer;
class GBuffer;
class VoxelGrid;
class VoxelShader;

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

	void RenderVoxelGrid(VoxelGrid* voxelGrid);

	void RenderSky();

	void CopyViewportToTexture(RenderTexture2D& texture);

	std::unique_ptr<GBuffer> gBuffer;
	std::unique_ptr<Framebuffer> deferredFramebuffer;

	std::unique_ptr<VoxelGrid> testVoxelGrid;
	std::unique_ptr<ArrayTexture> voxelTextures;

	Shader gBufferShader;
	std::unique_ptr<VoxelShader> voxelShader;
	std::unique_ptr<DeferredShader> deferredShader;
	Shader skyShader;


	LightUniformLocations lightUniformLocations;
	Light testLight;
	int materialColorLocation, materialRoughnessLocation = 0;

	Camera3D camera;
	RenderTexture2D viewportTexture;
	Material defaultMaterial;
	Model testModel;
};