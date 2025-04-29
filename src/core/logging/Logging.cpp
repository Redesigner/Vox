#include "Logging.h"

namespace Vox
{
	LogFilter::LogFilter()
	{
		categoryFilter.fill(true);
		levelFilter.fill(true);
	}

	bool LogFilter::GetIsFiltered(const LogEntry& entry) const
	{
		return categoryFilter[entry.category] && levelFilter[entry.level];
	}

	bool LogFilter::GetCategoryFilter(LogCategory category) const
	{
		return categoryFilter[category];
	}

	bool LogFilter::GetLevelFilter(LogLevel level) const
	{
		return levelFilter[level];
	}

	void LogFilter::SetCategoryFilter(LogCategory category, bool filter)
	{
		categoryFilter[category] = filter;
	}

	void LogFilter::SetLevelFilter(LogLevel level, bool filter)
	{
		levelFilter[level] = filter;
	}

	LogEntry::LogEntry(std::string entry, LogLevel level, LogCategory category)
		: entry(entry), level(level), category(category)
	{
	}

	std::vector<LogEntry> Logger::entries = std::vector<LogEntry>();

	std::vector<LogCategory> Logger::categories = {
		Config,
		FileSystem,
		Game,
		Input,
		Physics,
		Rendering
	};
	
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

		case Config:
			return "Config";
			
		default:
			return "Unknown";
		}
	}

	glm::vec3 Logger::GetLevelColor(LogLevel level)
	{
		switch (level)
		{
		case Error:
			return {220.0f, 15.0f, 15.0f};
		case Warning:
			return {250.0f, 245.0f, 45.0f};
		default:
			return {255.0f, 255.0f, 255.0f};
		}
	}

	std::vector<LogEntry>& Logger::GetEntries()
	{
		return entries;
	}

	std::vector<LogEntry> Logger::GetEntriesFiltered(LogFilter filter)
	{
		std::vector<LogEntry> result;
		for (const LogEntry& entry : entries)
		{
			if (filter.GetIsFiltered(entry))
			{
				result.emplace_back(entry);
			}
		}
		return result;
	}

	std::vector<LogCategory>& Logger::GetCategories()
	{
		return categories;
	}
}
