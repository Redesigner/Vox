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

        void WriteToFile(const std::string& filename, const std::string& data);

        [[nodiscard]] std::string LoadFile(const std::string& filename) const;
        
    private:
        std::string rootPath;
        std::string assetPath;
    };
}
