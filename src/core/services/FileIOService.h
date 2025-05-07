#pragma once
#include <string>

namespace Vox
{
    class FileIOService
    {
    public:
        const std::string& GetRootPath() const;
        
    private:
        std::string rootPath = "../../../";
    };
}
