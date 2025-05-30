#include "world.hpp"

namespace GearX {
	RTTR_REGISTRATION{
		using namespace rttr;
	};
	// 加载指定关卡
	void World::loadLevel(const std::string& level_url) {
		auto it = levels.find(level_url);
		if (it != levels.end()) {
			current_level_url = it->first;
			it->second->initTextureSize();
		}
		else {
			auto new_level = std::make_shared<Level>();
			static const std::regex jsonRegex(R"(\.(json)$)");
			if (std::regex_search(level_url, jsonRegex)) {
				new_level->load(level_url, true);
			}
			else {
				new_level->load(level_url, false);
			}
			levels[level_url] = new_level;
			current_level_url = new_level->getLevelURL();
			new_level->initTextureSize();
		}
	}

	// 获取当前关卡
	std::shared_ptr<Level> World::getCurrentLevel() {
		if (!current_level_url.empty())
			return levels[current_level_url];
		return nullptr;
	}

	// 创建新关卡
	std::shared_ptr<Level> World::createLevel(const std::string& level_url) {
		if (levels.find(level_url) != levels.end()) {
			return levels[level_url]; // 已存在则返回现有
		}
		auto new_level = std::make_shared<Level>();
		new_level->setLevelName(std::string(level_url));
		new_level->setLevelURL(std::string(level_url));
		levels[level_url] = new_level;
		return new_level;
	}

	// 设置当前关卡
	void World::setCurrentLevel(std::shared_ptr<Level> level) {
		levels[level->getLevelURL()] = level;
		current_level_url = level->getLevelURL();
	}
	void GearX::World::setCurrentLevel(std::string url) {
		if (levels.find(url) != levels.end()) {
			this->current_level_url = url;
		}
		else if (!url.empty()) {
			loadLevel(url);
		}
	}
	// 获取指定关卡
	std::shared_ptr<Level> World::getLevel(const std::string& level_url) {
		auto it = levels.find(level_url);
		return (it != levels.end()) ? it->second : nullptr;
	}

	// 获取世界文件路径
	std::string World::getWorldUrl() const {
		return world_url;
	}
	void World::setWorldUrl(std::string url) {
		world_url = url;
	}
	std::vector<std::string> GearX::World::getLevelsUrls() {
		std::vector<std::string> urls;
		for (auto& i : levels) {
			urls.push_back(i.first);
		}
		return urls;
	}
	// 获取当前关卡路径
	std::string World::getCurrentLevelUrl() const {
		static std::string empty_str;
		return current_level_url;
	}

	// 加载世界
	void World::loadWorld(const std::string& url, bool isJson) {
		std::ifstream ifs;
		this->saveWorld(this->world_url);
		this->clear();
		try {
			if (isJson) {
				ifs.open(url);
				if (ifs.is_open()) {
					cereal::JSONInputArchive archive(ifs);
					archive(*this);
				}
			}
			else {
				ifs.open(url, std::ios::binary);
				if (ifs.is_open()) {
					cereal::PortableBinaryInputArchive archive(ifs);
					archive(*this);
				}
			}
			this->world_url = url;
		}
		catch (std::exception e) {
			ifs.close();
			SDL_Log("World load failed: %s", e.what());
			return;
		}
	}
	void World::loadWorld(const std::string& world) {
		static const std::regex jsonRegex(R"(\.(json)$)");
		if (std::regex_search(world, jsonRegex)) {
			loadWorld(world, true);
		}
		else {
			loadWorld(world, false);
		}
	}
	// 保存世界
	void World::saveWorld(std::string url, bool isJson) {
		if (!url.empty() && url != "") {
			if (world_url.empty() || world_url == "") {
				if (isJson)
					world_url = "world.json";
				else
					world_url = "world.binary";
				std::string path = std::string(SDL_GetBasePath()) + "/" + world_url;
				world_url = path;
			}
		}
		if (isJson) {
			std::ofstream ofs(world_url);
			if (ofs.is_open()) {
				cereal::JSONOutputArchive archive(ofs);
				archive(*this);
			}
		}
		else {
			std::ofstream ofs(world_url, std::ios::binary);
			if (ofs.is_open()) {
				cereal::PortableBinaryOutputArchive archive(ofs);
				archive(*this);
			}
		}
	}

	void World::saveWorld(std::string world) {
		static const std::regex jsonRegex(R"(\.(json)$)");
		if (std::regex_search(world, jsonRegex)) {
			saveWorld(world, true);
		}
		else {
			saveWorld(world, false);
		}
	}
	// 初始化世界
	void World::initialize() {
		// 初始化资源/管理器等
	}

	// 清空世界数据
	void World::clear() {
		levels.clear();
		current_level_url.clear();
		world_url.clear();
	}

	// 卸载当前关卡
	void World::unloadCurrentLevel() {
		levels.erase(current_level_url);
		current_level_url.clear();
	}

	// 重新加载当前关卡
	void World::reloadCurrentLevel() {
		if (!current_level_url.empty()) {
			std::string url = current_level_url;
			unloadCurrentLevel();
			loadLevel(url);
		}
	}

	// 卸载指定关卡
	void World::unloadLevel(const std::string& level_url) {
		auto it = levels.find(level_url);
		if (it != levels.end()) {
			if (current_level_url == level_url) {
				current_level_url.clear();
			}
			// it->second->unload(); // 卸载逻辑
			levels.erase(it);
		}
	}

	// 重新加载指定关卡
	void World::reloadLevel(const std::string& level_name) {
		auto level = getLevel(level_name);
		if (level) {
			std::string name = level->getLevelName();
			unloadLevel(level_name);
			std::string url = level->getLevelURL();
			level.reset();
			level = createLevel(url);
			level->setLevelName(name);
			static const std::regex jsonRegex(R"(\.(json)$)");
			if (std::regex_search(url, jsonRegex)) {
				level->load(url, true);
			}
			else {
				level->load(url, false);
			}
		}
	}
} // namespace GearX