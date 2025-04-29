#pragma once

#include <optional>
#include <string>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Vox
{
	class Shader
	{
	public:
		~Shader();
		void Enable() const;
		static void Disable();

		int GetUniformLocation(const char* uniformName) const;
		int GetUniformLocation(const std::string& uniformName) const;
		static void SetUniformInt(int uniformLocation, int value);
		void SetUniformFloat(int uniformLocation, float value);
		void SetUniformVec3(int uniformLocation, glm::vec3 value);
		void SetUniformColor(int uniformLocation, glm::vec4 color);
		void SetUniformMatrix(int uniformLocation, glm::mat4x4 matrix);

		// TEMPORARY USAGE ONLY
		unsigned int GetId() const;

	protected:
		bool Link();
		std::optional<unsigned int> LoadShaderStage(std::string shaderFilePath, unsigned int shaderType);

		unsigned int shader = 0;
	};
}