#pragma once

#include <glm/vec3.hpp>

namespace Vox
{
	struct PBRMaterial
	{
		PBRMaterial(glm::vec4 albedo, float roughness, float metallic)
			:albedo(albedo), roughness(roughness), metallic(metallic)
		{
		}

		glm::vec4 albedo;
		float roughness;
		float metallic;
	};
}