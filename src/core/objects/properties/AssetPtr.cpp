//
// Created by steph on 6/8/2025.
//

#include "AssetPtr.h"

namespace Vox
{
    constexpr std::string AssetPtr::GetAssetTypeName(const AssetType assetType)
    {
        switch (assetType)
        {
        case AssetType::Mesh:
            return "Mesh";

        case AssetType::SkeletalMesh:
            return "SkeletalMesh";

        case AssetType::Texture:
            return "Texture";

        default:
        case AssetType::Invalid:
            return "Invalid";
        }
    }

    bool AssetPtr::operator==(const AssetPtr& other) const
    {
        return type == other.type && path == other.path;
    }

    AssetPtr::AssetType AssetPtr::GetAssetTypeFromString(const std::string& string)
    {
        if (string == GetAssetTypeName(AssetType::Mesh))
        {
            return AssetType::Mesh;
        }

        if (string == GetAssetTypeName(AssetType::SkeletalMesh))
        {
            return AssetType::SkeletalMesh;
        }

        if (string == GetAssetTypeName(AssetType::Texture))
        {
            return AssetType::Texture;
        }

        return AssetType::Invalid;
    }

    std::optional<AssetPtr> AssetPtr::Deserialize(const nlohmann::json& jsonObject)
    {
        if (!jsonObject.contains("assetType") || !jsonObject["assetType"].is_string())
        {
            return std::nullopt;
        }

        if (!jsonObject.contains("path") || !jsonObject["path"].is_string())
        {
            return std::nullopt;
        }

        AssetType type = GetAssetTypeFromString(jsonObject["assetType"]);
        if (type == AssetType::Invalid)
        {
            return std::nullopt;
        }

        return AssetPtr{type, std::filesystem::path(jsonObject["path"].get<std::string>())};
    }

    nlohmann::ordered_json AssetPtr::Serialize() const
    {
        using Json = nlohmann::ordered_json;
        Json assetPtrJson = Json::object();
        assetPtrJson["assetType"] = type;
        assetPtrJson["path"] = path.string();
        return assetPtrJson;
    }
} // Vox