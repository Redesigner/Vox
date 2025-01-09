#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Vox
{
	class Shader
	{
	public:
		void Enable();
		void Disable();

		int GetUniformLocation(const char* uniformName);
		void SetUniformInt(int uniformLocation, int value);
		void SetUniformFloat(int uniformLocation, float value);
		void SetUniformVec3(int uniformLocation, glm::vec3 value);
		void SetUniformColor(int uniformLocation, glm::vec4 color);
		void SetUniformMatrix(int uniformLocation, glm::mat4x4 matrix);

	protected:
		unsigned int shader;
	};
}