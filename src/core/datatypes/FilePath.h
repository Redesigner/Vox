//
// Created by steph on 6/8/2025.
//

#pragma once
#include <string>
#include <vector>

namespace Vox
{
    struct FilePath
    {
        [[nodiscard]] std::string String() const;
        void Push(const std::string& path);
        void Pop();

        [[nodiscard]] FilePath operator + (const FilePath& other) const;

    private:
        std::vector<std::string> internalData;
    };

} // Vox
