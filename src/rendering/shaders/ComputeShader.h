#pragma once

#include "rendering/shaders/Shader.h"

#include <string>

namespace Vox
{
	class ComputeShader : public Shader
	{
	public:
		bool Load(std::string shaderFilePath);

	private:
		unsigned int computeShader;
	};
}