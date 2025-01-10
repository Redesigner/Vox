#include "DeferredShader.h"

#include <GL/glew.h>

namespace Vox
{
	DeferredShader::DeferredShader()
	{
		if (!Load(vertLocation, fragLocation))
		{
			return;
		}

		uniformLocations.position = GetUniformLocation("gPosition");
		uniformLocations.normal = GetUniformLocation("gNormal");
		uniformLocations.albedo = GetUniformLocation("gAlbedo");
		uniformLocations.metallicRoughness = GetUniformLocation("gMetallicRoughness");
		uniformLocations.depth = GetUniformLocation("depth");
		uniformLocations.viewPosition = GetUniformLocation("viewPosition");

		Enable();
		SetUniformInt(uniformLocations.position, 0);
		SetUniformInt(uniformLocations.normal, 1);
		SetUniformInt(uniformLocations.albedo, 2);
		SetUniformInt(uniformLocations.metallicRoughness, 3);
		SetUniformInt(uniformLocations.depth, 4);

		/*
		shaderLocation = 1;
		rlSetUniformSampler(uniformLocations.normal, shaderLocation);
		rlSetUniform(uniformLocations.normal, &shaderLocation, SHADER_UNIFORM_INT, 1);*/
	}

	void DeferredShader::SetCameraPosition(glm::vec3 position)
	{
		SetUniformVec3(uniformLocations.viewPosition, position);
	}
}