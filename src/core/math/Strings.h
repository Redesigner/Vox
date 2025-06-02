//
// Created by steph on 5/28/2025.
//

#pragma once
#include <string>
#include <vector>

namespace Vox
{

    std::string IncrementString(const std::string& string);
    std::vector<std::string> SplitString(const std::string& string, char delimiter);

    //@TODO: rewrite this to use iterators instead of vector
    std::string JoinString(const std::vector<std::string>::const_iterator& begin, const std::vector<std::string>::const_iterator& end, char separator);

} // Vox
