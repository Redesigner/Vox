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

		default:
			return "Unknown";
		}
	}

	Color Logger::GetLevelColor(LogLevel level)
	{
		switch (level)
		{
		case Error:
			return Color(220.0f, 15.0f, 15.0f);
		case Warning:
			return Color(244.0f, 238.0f, 68.0f);
		default:
			return Color(255.0f, 255.0f, 255.0f);
		}
	}

	std::vector<LogEntry>& Logger::GetEntries()
	{
		return entries;
	}
}
