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

		[[nodiscard]] int GetUniformLocation(const char* uniformName) const;
		[[nodiscard]] int GetUniformLocation(const std::string& uniformName) const;
		static void SetUniformInt(int uniformLocation, int value);
		static void SetUniformUint(int uniformLocation, unsigned int value);
		static void SetUniformFloat(int uniformLocation, float value);
		static void SetUniformVec3(int uniformLocation, glm::vec3 value);
		static void SetUniformColor(int uniformLocation, glm::vec4 color);
		static void SetUniformMatrix(int uniformLocation, glm::mat4x4 matrix);

		// TEMPORARY USAGE ONLY
		unsigned int GetId() const;

	protected:
		bool Link();
		std::optional<unsigned int> LoadShaderStage(std::string shaderFilePath, unsigned int shaderType) const;

		unsigned int shader = 0;
	};
}