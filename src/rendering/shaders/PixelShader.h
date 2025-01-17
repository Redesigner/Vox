#pragma once

#include "rendering/shaders/Shader.h"

namespace Vox
{
	class PixelShader : public Shader
	{
	public:
		~PixelShader();

		bool Load(std::string vertexShaderFilePath, std::string fragmentShaderFilePath);

	protected:
		unsigned int vertexShader, fragmentShader;
	};
}