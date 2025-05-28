#include "asset_manager.hpp"
#include "../loader/texture_loader.hpp"
#include "../loader/sound_loader.hpp"
#include "../loader/font_loader.hpp"
#include "../loader/script_loader.hpp"

// 判断是否为音频文件
bool GearX::isMusicFile(const std::string& filename) {
	static const std::regex audioRegex(R"(\.(mp3|wav|flac|aac|ogg)$)");
	return std::regex_search(filename, audioRegex);
}
bool GearX::isChunkFile(const std::string& filename) {
	static const std::regex audioRegex(R"(\.(wav)$)");
	return std::regex_search(filename, audioRegex);
}

// 判断是否为图片文件
bool GearX::isImageFile(const std::string& filename) {
	static const std::regex imageRegex(R"(\.(jpg|jpeg|png|bmp|gif|webp)$)");
	return std::regex_search(filename, imageRegex);
}

// 判断是否为字体文件
bool GearX::isFontFile(const std::string& filename) {
	static const std::regex fontRegex(R"(\.(ttf|otf|woff|woff2)$)");
	return std::regex_search(filename, fontRegex);
}

bool GearX::isScriptFile(const std::string& filename) {
	static const std::regex scriptRegex(R"(\.(lua)$)");
	return std::regex_search(filename, scriptRegex);
}

GearX::AssetManager::~AssetManager(void) {
	for (auto i = m_AssetMap.begin(); i != m_AssetMap.end(); i++) {
		releaseAsset(i->first);
	}
	m_AssetMap.clear();
}

GearX::AssetManager::AssetMapType& GearX::AssetManager::getAllAsset()
{
	return m_AssetMap;
}

GearX::Asset GearX::AssetManager::loadAsset(const std::string& file) {
	if (m_AssetMap.count(file) > 0 && m_AssetMap[file].data) {
		return m_AssetMap[file];
	}
	if (isImageFile(file)) {
		return loadAssetTexture(file);
	}
	if (isMusicFile(file)) {
		return loadAssetChunk(file);
	}
	if (isMusicFile(file)) {
		return loadAssetMusic(file);
	}
	if (isFontFile(file)) {
		return loadAssetFont(file);
	}
	if (isScriptFile(file)) {
		return loadAssetScript(file);
	}

	return Asset();
}

GearX::Asset GearX::AssetManager::loadAssetTexture(const std::string& file) {
	if (DefaultTexture == nullptr) {
		DefaultTexture = loadTexture("./asset/default/DefaultTexture.png");
	}
	Asset asset{ AssetType::Texture, file,(void*)DefaultTexture };
	if (m_AssetMap.count(file) == 0 || m_AssetMap[file].data) {
		asset = { AssetType::Texture, file,(void*)loadTexture(file) };
		if (asset.data == nullptr) {
			asset.data = (void*)DefaultTexture;
			SDL_Log("Loaded : DefaultTexture");
		}
	}
	m_AssetMap[file] = asset;
	return m_AssetMap[file];
}

GearX::Asset GearX::AssetManager::loadAssetChunk(const std::string& file)
{
	if (m_AssetMap.count(file) == 0 || m_AssetMap[file].data) {
		Asset asset = { AssetType::Chunk, file,(void*)loadChunk(file) };
		m_AssetMap[file] = asset;
	}
	return m_AssetMap[file];
}

GearX::Asset  GearX::AssetManager::loadAssetMusic(const std::string& file)
{
	if (m_AssetMap.count(file) == 0 || m_AssetMap[file].data) {
		Asset asset = { AssetType::Music, file,(void*)loadMusic(file) };
		m_AssetMap[file] = asset;
	}
	return m_AssetMap[file];
}

GearX::Asset GearX::AssetManager::loadAssetFont(const std::string& file)
{
	if (m_AssetMap.count(file) == 0 || m_AssetMap[file].data) {
		Asset asset = { AssetType::Font, file,(void*)loadFont(file) };
		m_AssetMap[file] = asset;
	}
	return m_AssetMap[file];
}

GearX::Asset GearX::AssetManager::loadAssetScript(const std::string& file) {
	Asset asset{ AssetType::Script, file,(void*)nullptr };
	if (m_AssetMap.count(file) == 0 || m_AssetMap[file].data) {
		asset = { AssetType::Script, file,(void*)loadScript(file) };
		m_AssetMap[file] = asset;
	}
	return asset;
}

GearX::Asset GearX::AssetManager::reloadAssetScript(const std::string& file) {
	if (m_AssetMap.count(file) != 0) {
		static_cast<ScriptHolder*>(m_AssetMap[file].data)
			->reload(file);
		return m_AssetMap[file];
	}
	else {
		return loadAssetScript(file);
	}
}

GearX::Asset GearX::AssetManager::getDefaultTextureAsset()
{
	Asset asset = { AssetType::Texture, "DefaultTexture",(void*)DefaultTexture };
	return asset;
}

void GearX::AssetManager::releaseAsset(const std::string& file)
{
	auto i = m_AssetMap.find(file);
	if (i != m_AssetMap.end() && i->second.data != nullptr) {
		switch (i->second.type)
		{
		case AssetType::Texture:
			SDL_DestroyTexture(static_cast<SDL_Texture*>(i->second.data));
			break;
		case AssetType::Chunk:
			Mix_FreeChunk(static_cast<Mix_Chunk*>(i->second.data));
			break;
		case AssetType::Music:
			Mix_FreeMusic(static_cast<Mix_Music*>(i->second.data));
			break;
		case AssetType::Font:
			TTF_CloseFont(static_cast<TTF_Font*>(i->second.data));
			break;
		case AssetType::Script:
			delete static_cast<ScriptHolder*>(i->second.data);
		default:
			break;
		}
		i->second.data = nullptr;
	}
}