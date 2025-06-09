//
// Created by steph on 6/8/2025.
//

#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>

namespace Vox
{
    struct AssetPtr
    {
        enum class AssetType : char
        {
            Mesh,
            SkeletalMesh,
            Texture,
            Invalid
        };

        bool operator ==(const AssetPtr& other) const;

        static constexpr std::string GetAssetTypeName(AssetType assetType);
        static AssetType GetAssetTypeFromString(const std::string& string);

        static std::optional<AssetPtr> Deserialize(const nlohmann::json& jsonObject);

        nlohmann::ordered_json Serialize() const;

        AssetType type;
        std::filesystem::path path;
    };
} // Vox
