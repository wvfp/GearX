#pragma once
#include "../../core/global/global.hpp"
namespace GearX {
	class ScriptHolder {
	private:
		std::string _code;
		sol::state& lua;
	public:
		ScriptHolder(sol::state& _lua, const std::string& path) :lua(_lua) {
			_code = loadFile(path);
		}
		~ScriptHolder() {
			_code.clear();
		}
		void execute(sol::environment& env) {
			lua.safe_script(_code, env);
		}
		void reload(const std::string& path) {
			_code.clear();
			_code = loadFile(path);
		}
		std::string loadFile(const std::string& path) {
			std::ifstream file(path);
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			return content;
		}
		sol::state& getLuaState() { return lua; };
	};
	extern  ScriptHolder* loadScript(std::string url);
}