//
// Created by steph on 6/8/2025.
//

#include "FilePath.h"

#include "core/math/Strings.h"

namespace Vox
{
    std::string FilePath::String() const
    {
        return JoinString(internalData.begin(), internalData.end(), '/');
    }

    void FilePath::Push(const std::string& path)
    {
        internalData.emplace_back(path);
    }

    void FilePath::Pop()
    {
        if (internalData.empty())
        {
            return;
        }

        internalData.erase(internalData.end() - 1);
    }

    FilePath FilePath::operator+(const FilePath& other) const
    {
        FilePath result;
        result.internalData = internalData;
        result.internalData.emplace_back(other.internalData.begin(), other.internalData.end());
        return result;
    }
} // Vox