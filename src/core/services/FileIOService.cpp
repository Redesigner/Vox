#include "FileIOService.h"

#include <SDL3/SDL_iostream.h>

#include "core/logging/Logging.h"

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

    // ReSharper disable once CppMemberFunctionMayBeConst
    void FileIOService::WriteToFile(const std::string& filename, const std::string& data)
    {
        const std::string filepath = assetPath + filename;
        SDL_IOStream* fileStream = SDL_IOFromFile(filepath.c_str(), "w");
        if (!fileStream)
        {
            VoxLog(Error, FileSystem, "Failed to create new file '{}'.", filepath);
            return;
        }

        SDL_WriteIO(fileStream, data.c_str(), data.size());
        SDL_CloseIO(fileStream);
    }

    std::string FileIOService::LoadFile(const std::string& filename) const
    {
        const std::string filepath = assetPath + filename;
        SDL_IOStream* fileStream = SDL_IOFromFile(filepath.c_str(), "r");
        if (!fileStream)
        {
            VoxLog(Display, FileSystem, "Unable to open file '{}'", filepath);
            return "";
        }

        // @TODO find a better method, possibly using iterators
        std::string resultString;
        char byte;
        while (SDL_ReadIO(fileStream, &byte, 1))
        {
            resultString.push_back(byte);
        }
        SDL_CloseIO(fileStream);

        return resultString;
    }
}
