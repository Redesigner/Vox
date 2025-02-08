#pragma once

#include <string>

#include <glm/vec2.hpp>

namespace Vox
{
	class VoxConfig
	{
	public:
		VoxConfig();

		void Load();

		void Write();

	// private:
		static const std::string configFileLocation;

		glm::ivec2 windowPosition;
		glm::ivec2 windowSize;
	};
}