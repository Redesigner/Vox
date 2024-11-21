#pragma once

#include "rendering/shaders/Shader.h"

#include <string>

#include "raylib.h"

class ArrayTexture;

class VoxelShader : public VoxShader
{
public:
	struct UniformLocations
	{
		int modelMatrix, viewMatrix, projectionMatrix;
		int materialRoughness, materialMetallic;
		int colorTextures;
	};

	VoxelShader();
	~VoxelShader();

	void SetArrayTexture(ArrayTexture* arrayTexture);
	void SetModelMatrix(const Matrix& model);
	void SetViewMatrix(const Matrix& view);
	void SetProjectionMatrix(const Matrix& projection);

private:
	const std::string vertLocation = "assets/shaders/gBufferVoxel.vert";
	const std::string fragLocation = "assets/shaders/gBufferVoxel.frag";

	UniformLocations uniformLocations;
};