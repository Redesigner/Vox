#pragma once

#include <string>

#include <glm/vec3.hpp>

#include "rendering/shaders/Shader.h"

namespace Vox
{
	class DeferredShader : public Shader
	{
		struct UniformLocations
		{
			int position;
			int normal;
			int albedo;
			int metallicRoughness;
			int depth;
			int viewPosition;
		};

	public:
		DeferredShader();

		void SetCameraPosition(glm::vec3 position);

	private:
		UniformLocations uniformLocations;

		const std::string vertLocation = "assets/shaders/deferred.vert";
		const std::string fragLocation = "assets/shaders/deferred.frag";
	};
}