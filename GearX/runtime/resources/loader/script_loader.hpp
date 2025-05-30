#pragma once
#include "../../core/global/global.hpp"
namespace GearX {
	class ScriptHolder {
	private:
		std::string _code;
		bool isValid{ false };
		sol::state& lua;
	public:
		ScriptHolder(sol::state& _lua, const std::string& path) :lua(_lua) {
			isValid = false;
			_code = loadFile(path);
			sol::load_result r= lua.load(_code);
			sol::load_status s= r.status();
			switch (s)
			{
			case sol::load_status::ok:
				isValid = true;
				break;
			case sol::load_status::syntax:
				std::cout << "Syntax Error : " << path<<std::endl;
				break;
			case sol::load_status::memory:
				std::cout << "memory : " << path<<std::endl;
				break;
			case sol::load_status::gc:
				std::cout << "GC : " << path<<std::endl;
				break;
			case sol::load_status::file:
				std::cout << "File : " << path<<std::endl;
				break;
			default:
				break;
			}
		}
		~ScriptHolder() {
		}
		void execute(sol::environment& env) {
	    // 包装，调用
			if (isValid) {
				lua.script(_code, env);
			}
		}
		void reload(const std::string& path) {
			isValid = false;
			_code = loadFile(path);
			sol::load_status s = lua.load_buffer(_code.data(), _code.size()).status();
			switch (s)
			{
			case sol::load_status::ok:
				isValid = true;
				break;
			case sol::load_status::syntax:
				std::cout << "Syntax Error : " << path << std::endl;
				break;
			case sol::load_status::memory:
				std::cout << "memory : " << path << std::endl;
				break;
			case sol::load_status::gc:
				std::cout << "GC : " << path << std::endl;
				break;
			case sol::load_status::file:
				std::cout << "File : " << path << std::endl;
				break;
			default:
				break;
			}
		}
		std::string loadFile(const std::string& path) {
			std::ifstream file(path);
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			return content;
		}
		sol::state& getLuaState() { return lua; };
		bool isRegister{ false };
	};
	extern  ScriptHolder* loadScript(std::string url);
}