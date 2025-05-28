#pragma once
#include <string>
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

namespace GearX {
	enum class AssetType {
		None,
		Texture,
		Chunk,
		Music,
		Font,
		Script
	};
	class Asset {
	public:
		AssetType type = AssetType::None;
		std::string asset_url = "";
		void* data = NULL;
		template<class Archive>
		void serialize(Archive& archive) {
			archive(cereal::make_nvp("AssetUrl", asset_url));
		}
	};
}