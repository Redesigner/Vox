#include "Config.h"

#include <nlohmann/json.hpp>
#include <SDL3/SDL_iostream.h>

#include "core/logging/Logging.h"

// Config files should be human readable, so we want this to be in a set order
using json = nlohmann::ordered_json;

namespace Vox
{
	const std::string VoxConfig::configFileLocation = "../../../Config.json";

	VoxConfig::VoxConfig()
	{
		windowPosition = glm::ivec2(0, 0);
		windowSize = glm::ivec2(800, 450);
		windowMaximized = false;
	}

	void VoxConfig::Load()
	{
		// @TODO move file loading to an indirection so we can store extra variables, e.g. path
		SDL_IOStream* configStream = SDL_IOFromFile(configFileLocation.c_str(), "r");
		if (!configStream)
		{
			VoxLog(Display, Config, "Config file not found.");
			return;
		}

		// @TODO find a better method, possibly using iterators
		std::string configString;
		char byte;
		while (SDL_ReadIO(configStream, &byte, 1))
		{
			configString.push_back(byte);
		}
		SDL_CloseIO(configStream);

		if (configString.empty())
		{
			return;
		}

		json configJson = json::parse(configString);
		int64_t* x = configJson["window"]["x"].get_ptr<json::number_integer_t*>();
		int64_t* y = configJson["window"]["y"].get_ptr<json::number_integer_t*>();

		int64_t* w = configJson["window"]["w"].get_ptr<json::number_integer_t*>();
		int64_t* h = configJson["window"]["h"].get_ptr<json::number_integer_t*>();

		bool* configWindowMaximized = configJson["window"]["maximized"].get_ptr<bool*>();

		if (x && y)
		{
			windowPosition = glm::ivec2(*x, *y);
		}

		if (w && h)
		{
			windowSize = glm::ivec2(*w, *h);
		}


		if (configWindowMaximized)
		{
			windowMaximized = *configWindowMaximized;
		}
	}

	void VoxConfig::Write()
	{
		SDL_IOStream* configStream = SDL_IOFromFile(configFileLocation.c_str(), "w");
		if (!configStream)
		{
			VoxLog(Display, Config, "Failed to create config file.");
			return;
		}

		json configJson{};
		configJson["window"]["x"] = windowPosition.x;
		configJson["window"]["y"] = windowPosition.y;
		configJson["window"]["w"] = windowSize.x;
		configJson["window"]["h"] = windowSize.y;
		configJson["window"]["maximized"] = windowMaximized;

		int test = configJson["window"]["h"];
		std::string configString = configJson.dump(4);
		SDL_WriteIO(configStream, configString.c_str(), configString.size());
		SDL_CloseIO(configStream);
	}
}