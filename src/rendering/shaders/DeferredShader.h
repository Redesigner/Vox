#pragma once

#include <string>

#include "raylib.h"

class DeferredShader
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

	void Enable();
	void Disable();

	int GetUniformLocation(const char* uniformName);
	void SetUniformInt(int uniformLocation, int value);
	void SetUniformFloat(int uniformLocation, float value);
	void SetUniformVec3(int uniformLocation, Vector3 value);
	void SetUniformColor(int uniformLocation, Vector4 color);
	void SetCameraPosition(Vector3 position);

private:
	Shader deferredShader;
	UniformLocations uniformLocations;

	const std::string vertLocation = "assets/shaders/deferred.vert";
	const std::string fragLocation = "assets/shaders/deferred.frag";
};