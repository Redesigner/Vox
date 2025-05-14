#pragma once
#include <string>

namespace Vox
{
    class FileIOService
    {
    public:
        FileIOService();

        [[nodiscard]] const std::string& GetRootPath() const;
        [[nodiscard]] const std::string& GetAssetPath() const;
        
    private:
        std::string rootPath;
        std::string assetPath;
    };
}
