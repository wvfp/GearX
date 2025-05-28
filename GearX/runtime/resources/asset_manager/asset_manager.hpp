#pragma once
#include "asset.hpp"
#include "../../core/global/global.hpp"


namespace GearX {
	static SDL_Texture* DefaultTexture = nullptr;
	// �ж��Ƿ�Ϊ��Ƶ�ļ�
	extern bool isMusicFile(const std::string& filename);
	extern bool isChunkFile(const std::string& filename);
	// �ж��Ƿ�ΪͼƬ�ļ�
	extern bool isImageFile(const std::string& filename);
	// �ж��Ƿ�Ϊ�����ļ�
	extern bool isFontFile(const std::string& filename);
	// �ж��Ƿ�Ϊ�ű��ļ�
	extern bool isScriptFile(const std::string& filename);

	class AssetManager {
	private:
		using AssetMapType = std::map<std::string, Asset>;
		AssetMapType m_AssetMap;
	public:
		AssetManager() :m_AssetMap(AssetMapType()) {}
		~AssetManager(void);
		AssetMapType& getAllAsset();
		Asset loadAsset(const std::string& file);
		Asset loadAssetTexture(const std::string& file);
		Asset loadAssetChunk(const std::string& file);
		Asset loadAssetMusic(const std::string& file);
		Asset loadAssetFont(const std::string& file);
		Asset loadAssetScript(const std::string& file);
		Asset reloadAssetScript(const std::string& file);
		Asset getDefaultTextureAsset();
		void releaseAsset(const std::string& file);
	};
}