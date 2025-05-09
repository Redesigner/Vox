#pragma once

#include "rendering/shaders/pixel_shaders/PixelShader.h"

#include <string>

#include <glm/mat4x4.hpp>

#include "MeshShader.h"

namespace Vox
{
	class ArrayTexture;

	class VoxelShader : public MeshShader
	{
	public:
		struct UniformLocations
		{
			int viewPosition = -1;
			int materialRoughness = -1, materialMetallic = -1;
			int colorTextures = -1;
		};

		VoxelShader();

		void SetArrayTexture(ArrayTexture* arrayTexture) const;

	private:
		static inline std::string vertLocation = "assets/shaders/gBufferVoxel.vert";
		static inline std::string fragLocation = "assets/shaders/gBufferVoxel.frag";

		UniformLocations uniformLocations;
	};
}