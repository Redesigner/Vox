#pragma once

#include <array>
#include <string>
#include <vector>

#include <glm/vec3.hpp>
#include <fmt/format.h>

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
		Config,
		Game,
		Input,
		FileSystem
	};
	
	struct LogEntry
	{
		LogEntry(std::string entry, LogLevel level, LogCategory category);

		std::string entry;
		LogLevel level;
		LogCategory category;
	};

	struct LogFilter
	{
		LogFilter();
		
		bool GetIsFiltered(const LogEntry& entry) const;

		[[nodiscard]] bool GetCategoryFilter(LogCategory category) const;
		[[nodiscard]] bool GetLevelFilter(LogLevel level) const;
		
		void SetCategoryFilter(LogCategory category, bool filter);
		void SetLevelFilter(LogLevel level, bool filter);

	private:	
		std::array<bool, 6> categoryFilter;
		std::array<bool, 4> levelFilter;
	};
	
	class Logger
	{

	public:
		static glm::vec3 GetLevelColor(LogLevel level);

		template <typename... T>
		static void Log(LogLevel level, LogCategory category, fmt::format_string<T...> fmt, T&&... args)
		{
			std::string formatted = fmt::vformat(fmt, fmt::make_format_args(args...));
			std::string entry = '[' + GetCategoryTag(category) + "] " + formatted;
			glm::vec3 displayColor = GetLevelColor(level);
			printf("\033[38;2;%u;%u;%um%s\033[0m\n", static_cast<int>(displayColor.r), static_cast<int>(displayColor.g), static_cast<int>(displayColor.b), entry.c_str());
			entries.emplace_back(entry, level, category);
		}

		static std::vector<LogEntry>& GetEntries();

		static std::vector<LogEntry> GetEntriesFiltered(LogFilter filter);

		static std::vector<LogCategory>& GetCategories();
		
		static std::string GetCategoryTag(LogCategory category);

	private:
		static std::vector<LogEntry> entries;

		static std::vector<LogCategory> categories;
		
	};
}