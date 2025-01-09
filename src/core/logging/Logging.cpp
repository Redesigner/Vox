#include "Logging.h"

namespace Vox
{
	LogEntry::LogEntry(std::string entry, LogLevel level, LogCategory category)
		: entry(entry), level(level), category(category)
	{
	}

	std::vector<LogEntry> Logger::entries = std::vector<LogEntry>();

	std::string Logger::GetCategoryTag(LogCategory category)
	{
		switch (category)
		{
		case Physics:
			return "Physics";

		case Game:
			return "Game";

		case Rendering:
			return "Rendering";

		case Input:
			return "Input";

		case FileSystem:
			return "FileSystem";

		default:
			return "Unknown";
		}
	}

	glm::vec3 Logger::GetLevelColor(LogLevel level)
	{
		switch (level)
		{
		case Error:
			return glm::vec3(220.0f, 15.0f, 15.0f);
		case Warning:
			return glm::vec3(250.0f, 245.0f, 45.0f);
		default:
			return glm::vec3(255.0f, 255.0f, 255.0f);
		}
	}

	std::vector<LogEntry>& Logger::GetEntries()
	{
		return entries;
	}
}
