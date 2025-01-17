#pragma once

#include "rendering/shaders/PixelShader.h"

#include <string>

#include <glm/mat4x4.hpp>

namespace Vox
{
	class ArrayTexture;

	class VoxelShader : public PixelShader
	{
	public:
		struct UniformLocations
		{
			int modelMatrix, viewMatrix, projectionMatrix, viewPosition;
			int materialRoughness, materialMetallic;
			int colorTextures;
		};

		VoxelShader();

		void SetArrayTexture(ArrayTexture* arrayTexture);
		void SetModelMatrix(const glm::mat4x4& model);
		void SetViewMatrix(const glm::mat4x4& view);
		void SetProjectionMatrix(const glm::mat4x4& projection);

	private:
		const std::string vertLocation = "assets/shaders/gBufferVoxel.vert";
		const std::string fragLocation = "assets/shaders/gBufferVoxel.frag";

		UniformLocations uniformLocations;
	};
}