#include "VoxelShader.h"

#include <GL/glew.h>

#include "rendering/ArrayTexture.h"

namespace Vox
{
	VoxelShader::VoxelShader()
	{
		Load(vertLocation, fragLocation);
		uniformLocations.viewPosition = GetUniformLocation("viewPosition");

		uniformLocations.modelMatrix = GetUniformLocation("matModel");
		uniformLocations.viewMatrix = GetUniformLocation("matView");
		uniformLocations.projectionMatrix = GetUniformLocation("matProjection");

		uniformLocations.materialRoughness = GetUniformLocation("materialRoughness");
		uniformLocations.materialMetallic = GetUniformLocation("materialMetallic");
		uniformLocations.colorTextures = GetUniformLocation("colorTextures");
	}

	void VoxelShader::SetArrayTexture(ArrayTexture* arrayTexture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, arrayTexture->GetId());

		int position = 0;
		glUniform1i(uniformLocations.colorTextures, position);
	}

	void VoxelShader::SetModelMatrix(const glm::mat4x4& model)
	{
		SetUniformMatrix(uniformLocations.modelMatrix, model);
	}

	void VoxelShader::SetViewMatrix(const glm::mat4x4& view)
	{
		SetUniformMatrix(uniformLocations.viewMatrix, view);
	}

	void VoxelShader::SetProjectionMatrix(const glm::mat4x4& projection)
	{
		SetUniformMatrix(uniformLocations.projectionMatrix, projection);
	}
}