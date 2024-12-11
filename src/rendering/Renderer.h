#pragma once

#include "raylib.h"

#include <memory>
#include <string>

#include "rendering/Light.h"

class ArrayTexture;
class DebugRenderer;
class DeferredShader;
class Editor;
class Framebuffer;
class GBuffer;
class VoxelGrid;
class VoxelShader;

namespace Vox
{
	class Camera;
	class PhysicsServer;

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void Render(Editor* editor);

		void LoadTestModel(std::string path);

		void SetDebugPhysicsServer(std::shared_ptr<PhysicsServer> physicsServer);

		void SetCameraPosition(Vector3 position);

	private:
		void UpdateViewportDimensions(Editor* editor);

		void RenderGBuffer();

		void DrawMeshGBuffer(Mesh* mesh, Material* material, const Matrix& transform);

		void RenderDeferred();

		void RenderVoxelGrid(VoxelGrid* voxelGrid);

		void RenderSky();

		void RenderDebugShapes();

		void CopyViewportToTexture(RenderTexture2D& texture);

		std::unique_ptr<GBuffer> gBuffer;
		std::unique_ptr<Framebuffer> deferredFramebuffer;

		std::shared_ptr<VoxelGrid> testVoxelGrid;
		std::unique_ptr<ArrayTexture> voxelTextures;

		std::weak_ptr<Vox::PhysicsServer> debugPhysicsServer;

		Shader gBufferShader;
		std::unique_ptr<VoxelShader> voxelShader;
		std::unique_ptr<DeferredShader> deferredShader;
		Shader skyShader;
		Shader debugLineShader;
		int debugMatrixLocation = 0;

		LightUniformLocations lightUniformLocations;
		Light testLight;
		int materialColorLocation, materialRoughnessLocation = 0;

		std::unique_ptr<Vox::Camera> camera;

		RenderTexture2D viewportTexture;
		Material defaultMaterial;
		Model testModel;
	};
}