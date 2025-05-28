#pragma once
#include "../../../depencies.hpp"
#include "../level/level.hpp"
namespace GearX {
	class Level;
	class World {
	public:
		World() = default;
		~World() = default;
	public:
		void loadLevel(const std::string& level_url);
		std::shared_ptr<Level> getCurrentLevel();
		void setCurrentLevel(std::shared_ptr<Level> level);
		void setCurrentLevel(std::string url);
		std::shared_ptr<Level> createLevel(const std::string& level_name);
		std::shared_ptr<Level> getLevel(const std::string& level_name);
		std::string getWorldUrl()const;
		void setWorldUrl(std::string);
		std::vector<std::string> getLevelsUrls();
		std::string getCurrentLevelUrl()const;
		void loadWorld(const std::string& world_url, bool isJson);
		void loadWorld(const std::string& world);
		void saveWorld(std::string url, bool isJson);
		void saveWorld(std::string url);
		void initialize();
		void clear();
		void unloadCurrentLevel();
		void reloadCurrentLevel();
		void unloadLevel(const std::string& level_url);
		void reloadLevel(const std::string& level_url);
		template<typename Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("world_url", world_url));
			ar(cereal::make_nvp("current_level_url", current_level_url));
			std::vector<std::string> level_urls;
			for (auto& level : levels) {
				level_urls.push_back(level.first);
			}
			ar(cereal::make_nvp("level_urls", level_urls));
			if (typeid(Archive) == typeid(cereal::JSONInputArchive) ||
				typeid(Archive) == typeid(cereal::PortableBinaryInputArchive)) {
				for (auto& level : level_urls) {
					this->loadLevel(level);
				}
				//load current level
				this->setCurrentLevel(current_level_url);
			}
			else if (typeid(Archive) == typeid(cereal::JSONOutputArchive) ||
				typeid(Archive) == typeid(cereal::PortableBinaryOutputArchive)) {
				//save all levels
				for (auto& level : levels) {
					level.second->save();
				}
			}
		}

	private:
		std::string world_url;
		std::map<std::string, std::shared_ptr<Level>> levels;
		std::string current_level_url;
		RTTR_ENABLE()
			RTTR_REGISTRATION_FRIEND
	};
}
