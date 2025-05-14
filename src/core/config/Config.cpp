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
		windowPosition = glm::ivec2(100, 100);
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
	    if (configJson.contains("window"))
	    {
	        json& window = configJson["window"];
	        if (window.contains("x") && window["y"].is_number())
	        {
	            windowPosition.x = window["x"];
	        }
	        if (window.contains("y") && window["y"].is_number())
	        {
	            windowPosition.y = window["y"];
	        }
	        if (window.contains("w") && window["w"].is_number())
	        {
	            windowSize.x = window["w"];
	        }
	        if (window.contains("h") && window["h"].is_number())
	        {
	            windowSize.y = window["h"];
	        }
	        if (window.contains("maximized") && window["maximized"].is_boolean())
	        {
	            windowMaximized = window["maximized"];
	        }
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

		std::string configString = configJson.dump(4);
		SDL_WriteIO(configStream, configString.c_str(), configString.size());
		SDL_CloseIO(configStream);
	}
}