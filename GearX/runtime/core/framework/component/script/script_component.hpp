#pragma once
#include "../component.hpp"
#include "../../../utils/object_wrapper.hpp"
namespace GearX {
	class GObject;
	class Asset;
	class ScriptComponent : public Component {
	private:
		RTTR_ENABLE(GearX::Component)
			RTTR_REGISTRATION_FRIEND
	public:
		ScriptComponent() {
			m_type_name = "ScriptComponent";
		}
		std::shared_ptr<Component> copy() override {
			auto ptr = std::make_shared<ScriptComponent>();
			*ptr = *this;
			return ptr;
		}
		static void addComponentTo(std::shared_ptr<GObject> obj);
		void removeScript(std::string& url);
		void addScript(std::string& url);
		void removeScriptFromBeginContect(std::string& url);
		void addScriptToBeginContact(std::string& url);
		void removeScriptFromEndContect(std::string& url);
		void addScriptToEndContact(std::string& url);
		void addScriptToDoOnce(std::string& url);
		void removeScriptFromDoOnce(std::string& url);
		void reloadScripts();
		void clear();
		void loadScript();
		std::vector<Asset>& getScript() {
			return script_assets;
		}
		std::vector<Asset>& getScriptBeginContact() {
			return script_begin_contact;
		}
		std::vector<Asset>& getScriptEndContact() {
			return script_end_contact;
		}
		std::vector<std::string> getScriptUrls() {
			std::vector<std::string> urls;
			for (auto& asset : script_assets) {
				urls.push_back(asset.asset_url);
			}
			return urls;
		}
		std::vector<std::string> getScriptBeginContactUrls() {
			std::vector<std::string> urls;
			for (auto& asset : script_begin_contact) {
				urls.push_back(asset.asset_url);
			}
			return urls;
		}
		std::vector<std::string> getScriptEndContactUrls() {
			std::vector<std::string> urls;
			for (auto& asset : script_end_contact) {
				urls.push_back(asset.asset_url);
			}
			return urls;
		}
		std::vector<Asset>& getScriptDoOnce() {
			return script_do_once;
		}
		template<typename Archive>
		void serialize(Archive& archive) {
			Component::serialize(archive);
			archive(script_assets);
			archive(script_begin_contact);
			archive(script_end_contact);
			archive(script_do_once);
			if (typeid(archive) == typeid(cereal::JSONInputArchive)
				|| typeid(archive) == typeid(cereal::PortableBinaryInputArchive)) {
				loadScript();
			}
		}
	private:
		std::vector<Asset> script_assets;
		std::vector<Asset> script_begin_contact;
		std::vector<Asset> script_end_contact;
		std::vector<Asset> script_do_once;
	};
}
CEREAL_REGISTER_TYPE(GearX::ScriptComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(GearX::Component, GearX::ScriptComponent)