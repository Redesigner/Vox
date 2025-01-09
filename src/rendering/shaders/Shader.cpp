#include "Shader.h"

#include <GL/glew.h>

namespace Vox
{
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
}