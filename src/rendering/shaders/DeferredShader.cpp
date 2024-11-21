#include "DeferredShader.h"

#include "rlgl.h"

DeferredShader::DeferredShader()
{
	shader = LoadShader(vertLocation.c_str(), fragLocation.c_str());
	shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPosition");

	uniformLocations.position =				rlGetLocationUniform(shader.id, "gPosition");
	uniformLocations.normal =				rlGetLocationUniform(shader.id, "gNormal");
	uniformLocations.albedo =				rlGetLocationUniform(shader.id, "gAlbedo");
	uniformLocations.metallicRoughness =	rlGetLocationUniform(shader.id, "gMetallicRoughness");
	uniformLocations.depth =				rlGetLocationUniform(shader.id, "depth");
	uniformLocations.viewPosition =			rlGetLocationUniform(shader.id, "viewPosition");

	rlEnableShader(shader.id);
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
	if (IsShaderValid(shader))
	{
		UnloadShader(shader);
	}
}

void DeferredShader::SetCameraPosition(Vector3 position)
{
	float cameraPos[3] = { position.x, position.y, position.z };
	rlSetUniform(uniformLocations.viewPosition, &cameraPos, SHADER_UNIFORM_VEC3, 1);
}
