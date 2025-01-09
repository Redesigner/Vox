#pragma once

#include "raylib.h"

#include <fmt/format.h>
#include <string>
#include <vector>

#define VoxLog(level, category, string,...) Vox::Logger::Log(Vox::LogLevel::level, Vox::LogCategory::category, string, ##__VA_ARGS__)

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
		Game,
		Input
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
		static Color GetLevelColor(LogLevel level);

		template <typename... T>
		static void Log(LogLevel level, LogCategory category, fmt::format_string<T...> fmt, T&&... args)
		{
			std::string formatted = fmt::vformat(fmt, fmt::make_format_args(args...));
			std::string entry = '[' + GetCategoryTag(category) + "] " + formatted;
			Color displayColor = GetLevelColor(level);
			printf("\033[38;2;%u;%u;%um%s\033[0m\n", displayColor.r, displayColor.g, displayColor.b, entry.c_str());
			entries.emplace_back(entry, level, category);
		}

		static std::vector<LogEntry>& GetEntries();

	};
}