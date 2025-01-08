#include "Logging.h"

namespace Vox
{
	LogEntry::LogEntry(std::string entry, LogLevel level, LogCategory category)
		: entry(entry), level(level), category(category)
	{
	}

	std::string Logger::GetCategoryTag(LogCategory category)
	{
		switch (category)
		{
		case Physics:
		{
			return "Physics";
		}
		case Game:
		{
			return "Game";
		}
		case Rendering:
		{
			return "Rendering";
		}
		default:
		{
			return "Unknown";
		}
		}
	}

	std::vector<LogEntry>& Logger::GetEntries()
	{
		return entries;
	}
}
