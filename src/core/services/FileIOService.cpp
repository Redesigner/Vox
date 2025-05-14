#include "FileIOService.h"

namespace Vox
{
    FileIOService::FileIOService()
    {
        rootPath  = "../../../";
        assetPath = rootPath + "assets/";
    }

    const std::string& FileIOService::GetRootPath() const
    {
        return rootPath;
    }

    const std::string& FileIOService::GetAssetPath() const
    {
        return assetPath;
    }
}
