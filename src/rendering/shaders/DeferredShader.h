#pragma once

#include <string>

#include "raylib.h"

#include "rendering/shaders/Shader.h"

class DeferredShader : public VoxShader
{
	struct UniformLocations
	{
		int position;
		int normal;
		int albedo;
		int metallicRoughness;
		int depth;
		int viewPosition;
	};

public:
	DeferredShader();
	~DeferredShader();

	void SetCameraPosition(Vector3 position);

private:
	UniformLocations uniformLocations;

	const std::string vertLocation = "assets/shaders/deferred.vert";
	const std::string fragLocation = "assets/shaders/deferred.frag";
};