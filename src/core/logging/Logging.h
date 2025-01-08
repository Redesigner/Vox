#pragma once

#include <fmt/format.h>
#include <string>
#include <vector>

#define VoxLog(level, category, string,...) (Vox::Logger::Log(level, category, string, ##__VA_ARGS__))

namespace Vox
{
	enum LogLevel : char
	{
		Error,
		Warning,
		Display,
		Verbose
	};

	enum LogCategory : char
	{
		Physics,
		Rendering,
		Game
	};

	struct LogEntry
	{
		LogEntry(std::string entry, LogLevel level, LogCategory category);

		std::string entry;
		LogLevel level;
		LogCategory category;
	};

	class Logger
	{

	private:
		static std::string GetCategoryTag(LogCategory category);

		static std::vector<LogEntry> entries;

	public:
		template <typename... T>
		static void Log(LogLevel level, LogCategory category, fmt::format_string<T...> fmt, T&&... args)
		{
			std::string entry = '[' + GetCategoryTag(category) + "] " + fmt::format(fmt, args...);
			entries.emplace_back(entry, level, category);
		}

		static std::vector<LogEntry>& GetEntries();
	};
}