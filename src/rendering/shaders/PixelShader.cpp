#include "PixelShader.h"

#include <GL/glew.h>

#include "core/logging/Logging.h"

Vox::PixelShader::~PixelShader()
{
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

bool Vox::PixelShader::Load(std::string vertexShaderFilePath, std::string fragmentShaderFilePath)
{
	shader = glCreateProgram();

	std::optional<unsigned int> vertexShaderId = LoadShaderStage(vertexShaderFilePath, GL_VERTEX_SHADER);
	if (!vertexShaderId)
	{
		glDeleteProgram(shader);
		return false;
	}
	vertexShader = vertexShaderId.value();

	std::optional<unsigned int> fragmentShaderId = LoadShaderStage(fragmentShaderFilePath, GL_FRAGMENT_SHADER);
	if (!fragmentShaderId)
	{
		glDeleteProgram(shader);
		return false;
	}
	fragmentShader = fragmentShaderId.value();

	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	if (Link())
	{
		VoxLog(Display, Rendering, "ShaderProgram created successfully: ProgramId: '{}', ['{}', '{}']", shader, vertexShader, fragmentShader);
		return true;
	}
	return false;
}
