//
// Created by steph on 5/28/2025.
//

#include "Strings.h"

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
} // Vox