//
// Created by steph on 5/28/2025.
//

#include "Strings.h"

#include <algorithm>

namespace Vox
{
    std::string IncrementString(const std::string& string)
    {
        if (!std::isdigit(string.at(string.size() - 1)))
        {
            return string + '0';
        }

        int validNumIndex = 0;
        for (int i = string.size() - 1; i > 0; --i)
        {
            if (!std::isdigit(string.at(i)))
            {
                validNumIndex = i + 1;
                break;
            }
        }

        int stringNumberSuffix = std::stoi(string.substr(validNumIndex));
        std::string nameWithoutNum = string.substr(0, validNumIndex);
        nameWithoutNum.append(std::to_string(stringNumberSuffix + 1));
        return nameWithoutNum;
    }

    std::vector<std::string> SplitString(const std::string& string, const char delimiter)
    {
        std::vector<std::string> result;

        int currentSubstringIndex = 0;
        for (int i = 0; i < string.size(); ++i)
        {
            if (string[i] == delimiter)
            {
                result.emplace_back(string.substr(currentSubstringIndex, i - currentSubstringIndex));
                currentSubstringIndex = ++i;
            }
        }

        return result;
    }

    std::string JoinString(const std::vector<std::string>::const_iterator& begin, const std::vector<std::string>::const_iterator& end, const char separator)
    {
        std::string result;
        std::for_each(begin, end, [&result, separator](const auto& iterator)
        {
            if (!result.empty())
            {
                result += separator;
            }
            result.append(iterator);
        });

        return result;
    }
} // Vox