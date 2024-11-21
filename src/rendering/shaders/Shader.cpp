#include "Shader.h"

#include "rlgl.h"

void VoxShader::Enable()
{
	rlEnableShader(shader.id);
}

void VoxShader::Disable()
{
	rlDisableShader();
}

int VoxShader::GetUniformLocation(const char* uniformName)
{
	return rlGetLocationUniform(shader.id, uniformName);
}

void VoxShader::SetUniformInt(int uniformLocation, int value)
{
	rlSetUniform(uniformLocation, &value, RL_SHADER_UNIFORM_INT, 1);
}

void VoxShader::SetUniformFloat(int uniformLocation, float value)
{
	rlSetUniform(uniformLocation, &value, RL_SHADER_UNIFORM_FLOAT, 1);
}

void VoxShader::SetUniformVec3(int uniformLocation, Vector3 value)
{
	float values[3] = { value.x, value.y, value.z };
	rlSetUniform(uniformLocation, &values, RL_SHADER_UNIFORM_VEC3, 1);
}

void VoxShader::SetUniformColor(int uniformLocation, Vector4 color)
{
	float colorPacked[4] = {
		static_cast<float>(color.x) / 255.0f,
		static_cast<float>(color.y) / 255.0f,
		static_cast<float>(color.z) / 255.0f,
		static_cast<float>(color.w) / 255.0f
	};
	rlSetUniform(uniformLocation, &colorPacked, RL_SHADER_UNIFORM_VEC4, 1);
}