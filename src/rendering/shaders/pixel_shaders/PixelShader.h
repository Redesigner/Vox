#pragma once

#include "rendering/shaders/Shader.h"

namespace Vox
{
	class PixelShader : public Shader
	{
	public:
	    PixelShader(std::string vertexShaderFilePath, std::string fragmentShaderFilePath);
		~PixelShader();

	protected:
		unsigned int vertexShader = 0, fragmentShader = 0;

	private:
	    bool Load(std::string vertexShaderFilePath, std::string fragmentShaderFilePath);
	};
}