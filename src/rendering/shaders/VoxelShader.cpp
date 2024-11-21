#include "VoxelShader.h"

#include "rlgl.h"
#include "external/glad.h"

#include "rendering/ArrayTexture.h"

VoxelShader::VoxelShader()
{
	shader = LoadShader(vertLocation.c_str(), fragLocation.c_str());
	shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPosition");

	uniformLocations.modelMatrix =			rlGetLocationUniform(shader.id, "matModel");
	uniformLocations.viewMatrix =			rlGetLocationUniform(shader.id, "matView");
	uniformLocations.projectionMatrix =		rlGetLocationUniform(shader.id, "matProjection");

	uniformLocations.materialRoughness =	rlGetLocationUniform(shader.id, "materialRoughness");
	uniformLocations.materialMetallic =		rlGetLocationUniform(shader.id, "materialMetallic");
	uniformLocations.colorTextures =		rlGetLocationUniform(shader.id, "colorTextures");
}

VoxelShader::~VoxelShader()
{
	if (IsShaderValid(shader))
	{
		UnloadShader(shader);
	}
}

void VoxelShader::SetArrayTexture(ArrayTexture* arrayTexture)
{
	rlActiveTextureSlot(0);
	// Raylib doesn't support glBindTexture with arrays, so we need to call gl directly
	glBindTexture(GL_TEXTURE_2D_ARRAY, arrayTexture->GetId());

	int position = 0;
	rlSetUniformSampler(uniformLocations.colorTextures, position);
	rlSetUniform(uniformLocations.colorTextures, &position, SHADER_UNIFORM_INT, 1);
}

void VoxelShader::SetModelMatrix(const Matrix& model)
{
	rlSetUniformMatrix(uniformLocations.modelMatrix, model);
}

void VoxelShader::SetViewMatrix(const Matrix& view)
{
	rlSetUniformMatrix(uniformLocations.viewMatrix, view);
}

void VoxelShader::SetProjectionMatrix(const Matrix& projection)
{
	rlSetUniformMatrix(uniformLocations.viewMatrix, projection);
}
