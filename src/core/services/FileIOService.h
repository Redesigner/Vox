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
        [[nodiscard]] static std::string LoadFileAbsolutePath(const std::string& filepath);
        [[nodiscard]] static std::string GetBasePath();
        
    private:
        std::string rootPath;
        std::string assetPath;
    };
}
