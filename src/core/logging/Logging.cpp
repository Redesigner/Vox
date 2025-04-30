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

	bool LogFilter::operator==(const LogFilter& other) const
	{
		return categoryFilter == other.GetCategoryFilter() && levelFilter == other.GetLevelFilter();
	}

	bool LogFilter::IsCategoryDisplayed(LogCategory category) const
	{
		return categoryFilter[category];
	}

	bool LogFilter::IsLevelDisplayed(LogLevel level) const
	{
		return levelFilter[level];
	}

	const std::array<bool, 6>& LogFilter::GetCategoryFilter() const
	{
		return categoryFilter;
	}

	const std::array<bool, 4>& LogFilter::GetLevelFilter() const
	{
		return levelFilter;
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
	
	std::vector<LogLevel> Logger::levels = {
		Verbose,
		Display,
		Warning,
		Error
	};
	
	std::vector<LogCategory> Logger::categories = {
		Config,
		FileSystem,
		Game,
		Input,
		Physics,
		Rendering
	};

	size_t Logger::lastCachedSize = 0;
	LogFilter Logger::lastCachedFilter{};
	std::vector<size_t> Logger::cachedFilteredIndices{};
	
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
		}
		
		return "Unknown";
	}

	std::string Logger::GetLevelTag(LogLevel level)
	{
		switch (level)
		{
		case Verbose:
			return "Verbose";
			
		case Display:
			return "Display";

		case Warning:
			return "Warning";

		case Error:
			return "Error";
		}

		return "Unknown";
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

	std::vector<size_t> Logger::GetEntriesFilteredByIndex(LogFilter filter)
	{
		// Check if our last filter has changed, or more entries have been added
		if (filter == lastCachedFilter && entries.size() == lastCachedSize)
		{
			return cachedFilteredIndices;
		}

		lastCachedFilter = filter;
		lastCachedSize = entries.size();

		cachedFilteredIndices.clear();
		
		for (size_t i = 0; i < entries.size(); ++i)
		{
			if (filter.GetIsFiltered(entries[i]))
			{
				cachedFilteredIndices.emplace_back(i);
			}
		}
		return cachedFilteredIndices;
	}

	std::vector<LogCategory>& Logger::GetCategories()
	{
		return categories;
	}

	std::vector<LogLevel>& Logger::GetLevels()
	{
		return levels;
	}
}
