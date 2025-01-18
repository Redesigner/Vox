#include "ComputeShader.h"

#include <GL/glew.h>

#include "core/logging/Logging.h"

bool Vox::ComputeShader::Load(std::string shaderFilePath)
{
	shader = glCreateProgram();

	std::optional<unsigned int> computeShaderId = LoadShaderStage(shaderFilePath, GL_COMPUTE_SHADER);
	if (!computeShaderId)
	{
		glDeleteProgram(shader);
		return false;
	}
	computeShader = computeShaderId.value();

	glAttachShader(shader, computeShader);
	if (Link())
	{
		VoxLog(Display, Rendering, "Compute shader created successfully: ProgramId: '{}'", shader);
		return true;
	}
	return false;
}
