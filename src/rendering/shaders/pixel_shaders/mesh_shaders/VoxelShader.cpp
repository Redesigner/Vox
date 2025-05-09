#include "VoxelShader.h"

#include <GL/glew.h>

#include "rendering/buffers/ArrayTexture.h"

namespace Vox
{
	VoxelShader::VoxelShader()
		:MeshShader(vertLocation, fragLocation)
	{
		uniformLocations.viewPosition = GetUniformLocation("viewPosition");
		uniformLocations.materialRoughness = GetUniformLocation("materialRoughness");
		uniformLocations.materialMetallic = GetUniformLocation("materialMetallic");
		uniformLocations.colorTextures = GetUniformLocation("colorTextures");
	}

	void VoxelShader::SetArrayTexture(ArrayTexture* arrayTexture) const
    {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, arrayTexture->GetId());

		int position = 0;
		glUniform1i(uniformLocations.colorTextures, position);
	}
}