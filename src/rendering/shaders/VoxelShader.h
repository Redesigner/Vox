#pragma once

#include "rendering/shaders/Shader.h"

#include <string>

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

private:
	const std::string vertLocation = "assets/shaders/gBufferVoxel.vert";
	const std::string fragLocation = "assets/shaders/gBufferVoxel.frag";

	UniformLocations uniformLocations;
};