#pragma once

#include "rendering/shaders/Shader.h"

#include <string>

#include <glm/mat4x4.hpp>

namespace Vox
{
	class ArrayTexture;

	class VoxelShader : public Shader
	{
	public:
		struct UniformLocations
		{
			int modelMatrix, viewMatrix, projectionMatrix, viewPosition;
			int materialRoughness, materialMetallic;
			int colorTextures;
		};

		VoxelShader();
		~VoxelShader();

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