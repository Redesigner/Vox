#include "DeferredShader.h"

#include "rlgl.h"

DeferredShader::DeferredShader()
{
	deferredShader = LoadShader(vertLocation.c_str(), fragLocation.c_str());
	deferredShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(deferredShader, "viewPosition");

	uniformLocations.position =				rlGetLocationUniform(deferredShader.id, "gPosition");
	uniformLocations.normal =				rlGetLocationUniform(deferredShader.id, "gNormal");
	uniformLocations.albedo =				rlGetLocationUniform(deferredShader.id, "gAlbedo");
	uniformLocations.metallicRoughness =	rlGetLocationUniform(deferredShader.id, "gMetallicRoughness");
	uniformLocations.depth =				rlGetLocationUniform(deferredShader.id, "depth");
	uniformLocations.viewPosition =			rlGetLocationUniform(deferredShader.id, "viewPosition");

	rlEnableShader(deferredShader.id);
	{
		int shaderLocation = 0;
		rlSetUniformSampler(uniformLocations.position, shaderLocation);
		rlSetUniform(uniformLocations.position, &shaderLocation, SHADER_UNIFORM_INT, 1);

		shaderLocation = 1;
		rlSetUniformSampler(uniformLocations.normal, shaderLocation);
		rlSetUniform(uniformLocations.normal, &shaderLocation, SHADER_UNIFORM_INT, 1);

		shaderLocation = 2;
		rlSetUniformSampler(uniformLocations.albedo, shaderLocation);
		rlSetUniform(uniformLocations.albedo, &shaderLocation, SHADER_UNIFORM_INT, 1);

		shaderLocation = 3;
		rlSetUniformSampler(uniformLocations.metallicRoughness, shaderLocation);
		rlSetUniform(uniformLocations.metallicRoughness, &shaderLocation, SHADER_UNIFORM_INT, 1);

		shaderLocation = 4;
		rlSetUniformSampler(uniformLocations.depth, shaderLocation);
		rlSetUniform(uniformLocations.depth, &shaderLocation, SHADER_UNIFORM_INT, 1);

		rlDisableShader();
	}
}

DeferredShader::~DeferredShader()
{
	if (IsShaderValid(deferredShader))
	{
		UnloadShader(deferredShader);
	}
}

void DeferredShader::Enable()
{
	rlEnableShader(deferredShader.id);
}

void DeferredShader::Disable()
{
	rlDisableShader();
}

int DeferredShader::GetUniformLocation(const char* uniformName)
{
	return rlGetLocationUniform(deferredShader.id, uniformName);
}

void DeferredShader::SetUniformInt(int uniformLocation, int value)
{
	rlSetUniform(uniformLocation, &value, RL_SHADER_UNIFORM_INT, 1);
}

void DeferredShader::SetUniformFloat(int uniformLocation, float value)
{
	rlSetUniform(uniformLocation, &value, RL_SHADER_UNIFORM_FLOAT, 1);
}

void DeferredShader::SetUniformVec3(int uniformLocation, Vector3 value)
{
	float values[3] = {value.x, value.y, value.z};
	rlSetUniform(uniformLocation, &values, RL_SHADER_UNIFORM_VEC3, 1);
}

void DeferredShader::SetUniformColor(int uniformLocation, Vector4 color)
{
	float colorPacked[4] = {
		static_cast<float>(color.x) / 255.0f,
		static_cast<float>(color.y) / 255.0f,
		static_cast<float>(color.z) / 255.0f,
		static_cast<float>(color.w) / 255.0f
	};
	rlSetUniform(uniformLocation, &colorPacked, RL_SHADER_UNIFORM_VEC4, 1);
}

void DeferredShader::SetCameraPosition(Vector3 position)
{
	float cameraPos[3] = { position.x, position.y, position.z };
	rlSetUniform(uniformLocations.viewPosition, &cameraPos, SHADER_UNIFORM_VEC3, 1);
}
