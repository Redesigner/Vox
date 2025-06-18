#pragma once

#include "rendering/shaders/Shader.h"

#include <string>

namespace Vox
{
	class ComputeShader : public Shader
	{
	public:
        explicit ComputeShader(const std::string& shaderFilePath);
	};
}