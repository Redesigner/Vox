#include "FileIOService.h"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_iostream.h>

#include "core/logging/Logging.h"
#include "core/math/Strings.h"

namespace Vox
{
    FileIOService::FileIOService()
    {
        // rootPath  = "../../../";
        std::vector<std::string> basePathString = SplitString(GetBasePath(), '\\');
        rootPath = JoinString(basePathString.begin(), basePathString.end() - 3, '/') + '/';
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
        return LoadFileAbsolutePath(filepath);
    }

    std::string FileIOService::LoadFileAbsolutePath(const std::string& filepath)
    {
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

    std::string FileIOService::GetBasePath()
    {
        return {SDL_GetBasePath()};
    }
}
