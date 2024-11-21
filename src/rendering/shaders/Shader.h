#pragma once

#include "raylib.h"

class VoxShader
{
public:
	void Enable();
	void Disable();

	int GetUniformLocation(const char* uniformName);
	void SetUniformInt(int uniformLocation, int value);
	void SetUniformFloat(int uniformLocation, float value);
	void SetUniformVec3(int uniformLocation, Vector3 value);
	void SetUniformColor(int uniformLocation, Vector4 color);

protected:
	Shader shader;
};