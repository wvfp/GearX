#include "../../core/system/script/script_system.hpp"
#include "script_loader.hpp"
GearX::ScriptHolder* GearX::loadScript(std::string url) {
	namespace fs = std::filesystem;
	auto& lua = GearX::RuntimeGlobalContext::scriptSystem.getLuaState();
	if (fs::exists(url) && !fs::is_directory(url)) {
		return new ScriptHolder(lua,url);
	}
	return nullptr;
}