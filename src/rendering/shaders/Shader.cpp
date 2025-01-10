#include "Shader.h"

#include <GL/glew.h>
#include <SDL3/SDL_iostream.h>

#include "core/logging/Logging.h"

namespace Vox
{
	bool Shader::Load(std::string vertexShaderFilePath, std::string fragmentShaderFilePath)
	{
		shader = glCreateProgram();

		std::optional<unsigned int> vertexShaderId = LoadShaderStage(vertexShaderFilePath, GL_VERTEX_SHADER);
		if (!vertexShaderId)
		{
			glDeleteProgram(shader);
			return false;
		}

		std::optional<unsigned int> fragmentShaderId = LoadShaderStage(fragmentShaderFilePath, GL_FRAGMENT_SHADER);
		if (!fragmentShaderId)
		{
			glDeleteProgram(shader);
			return false;
		}

		glAttachShader(shader, vertexShaderId.value());
		glAttachShader(shader, fragmentShaderId.value());
		glLinkProgram(shader);

		GLint isLinked = 0;
		glGetProgramiv(shader, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
			std::string logString = std::string(infoLog.begin(), infoLog.end());
			VoxLog(Error, Rendering, "Failed to link shader: {}", logString);

			glDeleteProgram(shader);
			return false;
		}

		VoxLog(Display, Rendering, "Shader created successfully.");
		return true;
	}

	void Shader::Enable()
	{
		glUseProgram(shader);
	}

	void Shader::Disable()
	{
		glUseProgram(0);
	}

	int Shader::GetUniformLocation(const char* uniformName)
	{
		return glGetUniformLocation(shader, uniformName);
	}

	void Shader::SetUniformInt(int uniformLocation, int value)
	{
		glUniform1i(uniformLocation, value);
	}

	void Shader::SetUniformFloat(int uniformLocation, float value)
	{
		glUniform1f(uniformLocation, value);
	}

	void Shader::SetUniformVec3(int uniformLocation, glm::vec3 value)
	{
		glUniform3f(uniformLocation, value.x, value.y, value.z);
	}

	void Shader::SetUniformColor(int uniformLocation, glm::vec4 color)
	{
		glUniform4f(uniformLocation, color.r, color.g, color.b, color.a);
	}

	void Shader::SetUniformMatrix(int uniformLocation, glm::mat4x4 matrix)
	{
		glUniformMatrix4fv(uniformLocation, 1, false, &matrix[0][0]);
	}

	std::optional<unsigned int> Shader::LoadShaderStage(std::string shaderFilePath, unsigned int shaderType)
	{
		SDL_IOStream* shaderIO = SDL_IOFromFile(shaderFilePath.c_str(), "r");
		if (!shaderIO)
		{
			VoxLog(Error, FileSystem, "Failed to load shader file. SDL: {}", SDL_GetError());
			return std::nullopt;
		}

		void* shaderSource = SDL_LoadFile_IO(shaderIO, nullptr, true);
		if (!shaderSource)
		{
			VoxLog(Error, FileSystem, "Failed to load shader file. SDL: {}", SDL_GetError());
			return std::nullopt;
		}

		char* shaderString = static_cast<char*>(shaderSource);

		unsigned int shaderId = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(shaderId, 1, &shaderString, 0);
		glCompileShader(shaderId);

		GLint shaderCompilationResult = GL_FALSE;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &shaderCompilationResult);
		if (shaderCompilationResult == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
			std::string logString = std::string(errorLog.begin(), errorLog.end());
			VoxLog(Error, Rendering, "Shader failed to compile: {}", logString);

			glDeleteShader(shaderId);
			return std::nullopt;
		}

		return shaderId;
	}
}